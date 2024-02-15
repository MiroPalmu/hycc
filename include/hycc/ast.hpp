#pragma once

#include <array>
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

class identifier_node {};
class function_argument_node {};
class type_node {};
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

    constexpr void match_patterns(parser_t& parser);
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

        match_patterns(parser);

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

constexpr void scope_node::match_patterns(parser_t& parser) {
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
