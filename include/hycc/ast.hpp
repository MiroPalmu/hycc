#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <memory>
#include <ranges>
#include <span>
#include <utility>
#include <variant>
#include <vector>

#include "hycc/parser.hpp"
#include "hycc/tokenizer.hpp"

namespace hycc {
namespace ast {

// Forward declerations:
class identifier_node;
class function_argument_node;
class type_node;
class expression_node;
class class_decleration_node;
class function_decleration_node;
class namespace_decleration_node;
class data_decleration_node;
class decleration_parsing_node;
class scope_node;
class if_statement_node;
class for_loop_statement_node;
class break_statement_node;
class continue_statement_node;
class return_statement_node;
class nested_scope;
class statement_scope;
class namespace_scope;
class function_scope;
class class_scope;
using ordered_property = std::variant<nested_scope,
                                      if_statement_node,
                                      for_loop_statement_node,
                                      break_statement_node,
                                      continue_statement_node,
                                      return_statement_node,
                                      namespace_decleration_node,
                                      expression_node,
                                      data_decleration_node>;

using unordered_property = std::variant<function_decleration_node, class_decleration_node>;

template<std::size_t N>
using node_pattern = std::array<token_pattern, N>;

/// Represents :: in some qulified identifier.
struct scope_resolution_operator {};

using identifier_unit = std::variant<scope_resolution_operator, token>;

class identifier_node {
    static constexpr auto scope_resolution_operator_pattern =
        std::array{ token_pattern{ token_type::semantic_scope_operator, u8":" },
                    token_pattern{ token_type::semantic_scope_operator, u8":" } };

    static constexpr auto identifier_pattern = std::array{ token_type::identifier };

    bool stop_signal_                              = false;
    std::vector<identifier_unit> identifier_units_ = {};

    constexpr void match_single_pattern(parser_t& parser);

  public:
    constexpr void push(parser_t& parser) {
        // Ignore potential whitespace in the beginning.
        [[maybe_unused]] auto _ =
            parser.match_and_consume(std::vector{ token_type::whitespace }, false);

        while (not stop_signal_) { match_single_pattern(parser); }
        if (identifier_units_.empty()) parser.throw_syntax_error();
    }

    [[nodiscard]] friend constexpr bool operator==(const identifier_node& lhs,
                                                   const identifier_node& rhs) noexcept {
        if (lhs.identifier_units_.size() != rhs.identifier_units_.size()) return false;
        namespace rv = std::ranges::views;
        return std::ranges::all_of(
            rv::zip(lhs.identifier_units_, rhs.identifier_units_)
                | rv::transform([](const auto& x) {
                      const auto l = std::get<0>(x);
                      const auto r = std::get<1>(x);
                      if (std::holds_alternative<scope_resolution_operator>(l)
                          and std::holds_alternative<scope_resolution_operator>(r)) {
                          // Both are scope resolution operators.
                          return true;
                      } else if (std::holds_alternative<scope_resolution_operator>(l)
                                 or std::holds_alternative<scope_resolution_operator>(r)) {
                          // Only other is.
                          return false;
                      }
                      // Both are tokens and assumend to be a identifiers.
                      const auto l_t = std::get<token>(l);
                      const auto r_t = std::get<token>(r);
                      return l_t.sv_in_source == r_t.sv_in_source;
                  }),
            std::identity{});
    }
};
class function_argument_node {};

class type_node {
    static constexpr auto const_pattern =
        std::array{ token_pattern{ token_type::identifier, u8"const" } };

    static constexpr auto pointer_pattern =
        std::array{ token_pattern{ token_type::operator_token, u8"*" } };

    bool is_const_ = false;
    /// Forward decleration due to type_node being incomplete type.
    //     struct function_t_;
    //     std::optional<function_t_> function_;
    std::unique_ptr<type_node> pointed_type_{};
    std::optional<identifier_node> regular_type_{};

    constexpr void match_all_patterns(parser_t& parser);

  public:
    constexpr void push(parser_t& parser) { match_all_patterns(parser); }
    // [[nodiscard]] constexpr bool is_function() noexcept { return regular_type_.has_value(); }
    [[nodiscard]] constexpr bool is_pointer() noexcept { return static_cast<bool>(pointed_type_); }
    [[nodiscard]] constexpr auto pointed_type() -> type_node& {
        if (not is_pointer()) throw std::runtime_error{ "Trying to access null ptr!" };
        else
            return *pointed_type_;
    }
    [[nodiscard]] constexpr bool is_const() noexcept { return is_const_; }
    [[nodiscard]] constexpr bool is_regular_type() noexcept { return regular_type_.has_value(); }
};

// struct type_node::function_t_ {
//     function_argument_node args_;
//     type_node return_type_;
// };

class expression_node {};
class class_decleration_node {};
class function_decleration_node {};
class namespace_decleration_node {};
class data_decleration_node {};
class decleration_parsing_node {};
class scope_node {
    static constexpr auto nested_scope_pat =
        std::array{ token_pattern{ token_type::semantic_scope_operator, u8"{" } };
    static constexpr auto end_of_scope =
        std::array{ token_pattern{ token_type::semantic_scope_operator, u8"}" } };

    constexpr void match_single_pattern(parser_t& parser);
    std::vector<ordered_property> ordered_property_;
    bool is_global_scope_ = false;
    bool stop_signal_     = false;

  public:
    constexpr void mark_as_global_scope() noexcept { is_global_scope_ = true; }
    [[nodiscard]] constexpr bool is_global_scope(this auto&& self) noexcept {
        return self.is_global_scope_;
    }
    constexpr void push(parser_t& parser) {
    new_match:
        if (is_global_scope() and parser.all_parsed()) goto stop;
        if (stop_signal_) goto stop;

        match_single_pattern(parser);

        goto new_match;
    stop:
    }
    [[nodiscard]] constexpr decltype(auto) get_ordered_property(this auto&& self);
};
class if_statement_node {};
class for_loop_statement_node {};
class break_statement_node {};
class continue_statement_node {};
class return_statement_node {};

class nested_scope : public scope_node {};
class statement_scope : public scope_node {};
class namespace_scope : public scope_node {};
class function_scope : public scope_node {};
class class_scope : public scope_node {};

// Now that all classes are fully defined we can use them in members.

constexpr void identifier_node::match_single_pattern(parser_t& parser) {
    auto matched = decltype(parser.match_and_consume(scope_resolution_operator_pattern)){};
    if ((matched = parser.match_and_consume(scope_resolution_operator_pattern, false))) {
        identifier_units_.push_back(scope_resolution_operator{});
    } else if ((matched = parser.match_and_consume(identifier_pattern, false))) {
        identifier_units_.push_back(matched.value().front());
    } else {
        stop_signal_ = true;
    }
}

constexpr void type_node::match_all_patterns(parser_t& parser) {
    auto matched = parser_t::matched_type{};
    if ((matched = parser.match_and_consume(const_pattern))) { is_const_ = true; }
    if ((matched = parser.match_and_consume(pointer_pattern))) {
        pointed_type_ = std::make_unique<type_node>();
        pointed_type_->push(parser);
        return;
    } else {
        regular_type_ = identifier_node{};
        regular_type_.value().push(parser);
        return;
    }
}

constexpr void scope_node::match_single_pattern(parser_t& parser) {
    auto matched = decltype(parser.match_and_consume(nested_scope_pat)){};
    if ((matched = parser.match_and_consume(nested_scope_pat))) {
        auto scope = nested_scope{};
        scope.push(parser);
        ordered_property_.push_back(scope);
    } else if ((matched = parser.match_and_consume(end_of_scope))) {
        if (is_global_scope()) parser.throw_syntax_error();
        stop_signal_ = true;
    } else {
        parser.throw_syntax_error();
    }
}
[[nodiscard]] constexpr decltype(auto) scope_node::get_ordered_property(this auto&& self) {
    return std::span{ self.ordered_property_ };
}

} // namespace ast
} // namespace hycc
