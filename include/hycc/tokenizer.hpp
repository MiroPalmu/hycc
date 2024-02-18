#pragma once

#include <algorithm>
#include <array>
#include <bits/ranges_base.h>
#include <concepts>
#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

#include "hycc/sstd.hpp"
#include "hycc/state_pattern_matcher.hpp"

namespace hycc {

enum class char_class {
    whitespace,
    id,
    integer,
    literal_scope_operator,
    semantic_scope_operator,
    operator_unit,
    other
};

[[nodiscard]] constexpr auto classify_char(const char8_t c) -> char_class {
    using enum char_class;
    switch (c) {
        case (0x0009):
        case (0x000B):
        case (0x000C):
        case (0x0020):
        case (0x000A): return whitespace;
        case (0x0041):
        case (0x0042):
        case (0x0043):
        case (0x0044):
        case (0x0045):
        case (0x0046):
        case (0x0047):
        case (0x0048):
        case (0x0049):
        case (0x004A):
        case (0x004B):
        case (0x004C):
        case (0x004D):
        case (0x004E):
        case (0x004F):
        case (0x0050):
        case (0x0051):
        case (0x0052):
        case (0x0054):
        case (0x0055):
        case (0x0056):
        case (0x0057):
        case (0x0058):
        case (0x0059):
        case (0x005A):
        case (0x005F):
        case (0x0061):
        case (0x0062):
        case (0x0063):
        case (0x0064):
        case (0x0065):
        case (0x0066):
        case (0x0067):
        case (0x0068):
        case (0x0069):
        case (0x006A):
        case (0x006B):
        case (0x006C):
        case (0x006D):
        case (0x006E):
        case (0x006F):
        case (0x0070):
        case (0x0071):
        case (0x0072):
        case (0x0073):
        case (0x0074):
        case (0x0075):
        case (0x0076):
        case (0x0077):
        case (0x0078):
        case (0x0079):
        case (0x007A): return id;
        case (0x0030):
        case (0x0031):
        case (0x0032):
        case (0x0033):
        case (0x0034):
        case (0x0035):
        case (0x0036):
        case (0x0037):
        case (0x0038):
        case (0x0039): return integer;
        case (0x0022):
        case (0x0027):
        case (0x0060): return literal_scope_operator;
        case (0x0028):
        case (0x0029):
        case (0x002C):
        case (0x002E):
        case (0x003A):
        case (0x003B):
        case (0x005B):
        case (0x005D):
        case (0x007B):
        case (0x007D): return semantic_scope_operator;
        case (0x0021):
        case (0x0023):
        case (0x0024):
        case (0x0025):
        case (0x0026):
        case (0x002A):
        case (0x002B):
        case (0x002D):
        case (0x002F):
        case (0x003C):
        case (0x003D):
        case (0x003E):
        case (0x003F):
        case (0x0040):
        case (0x005C):
        case (0x005E):
        case (0x007C):
        case (0x007E): return operator_unit;
        default: return other;
    }
}

class source_code {
    sstd::ownership<std::u8string> code_;

  public:
    [[nodiscard]] constexpr source_code(std::u8string&& input)
        : code_{ sstd::make_shared_object<std::u8string>(std::move(input)) } {}

    [[nodiscard]] constexpr auto sv(this auto&& me) -> std::u8string_view {
        return me.code_.value();
    }
    [[nodiscard]] constexpr auto get_ownership_of_code() { return code_; }
};

enum class token_type {
    whitespace,
    identifier,
    integer,
    literal,
    semantic_scope_operator,
    operator_token, // operator is keyword :(
    error
};

constexpr auto get_name(const token_type t) -> std::string {
    switch (t) {
        case token_type::whitespace: return { "whitespace" };
        case token_type::identifier: return { "identifier" };
        case token_type::integer: return { "integer" };
        case token_type::literal: return { "literal" };
        case token_type::semantic_scope_operator: return { "semantic_scope_operator" };
        case token_type::operator_token: return { "operator_token" };
        case token_type::error: return { "error" };
        default: throw std::logic_error("All token_types are not present in get_name!");
    }
}

struct token {
    token_type type;
    /// Token can not outlive the code it referes to.
    [[maybe_unused]] sstd::ownership<std::u8string> source;
    std::u8string_view sv_in_source;
    std::size_t row;
    std::size_t column;
};

struct tokenize_state {
    std::vector<token> tokens = std::vector<token>{};
    using marker_type         = std::u8string_view::const_iterator;

    marker_type current_pos;
    marker_type source_end;

    sstd::ownership<std::u8string> source_ownership;

    std::size_t current_row = 0;
    /// "Virtual newline" one first row is on column 0.
    std::size_t current_column = 1;

    struct {
        std::size_t row;
        std::size_t column;
        marker_type start_pos;
    } cache;

    constexpr void set_cache() {
        cache.row       = current_row;
        cache.column    = current_column;
        cache.start_pos = current_pos;
    }

    constexpr auto cache_as_sv(this auto&& self) -> std::u8string_view {
        return std::u8string_view{ self.cache.start_pos, self.current_pos };
    }

    constexpr auto potential_next_cache_as_sv(this auto&& self) -> std::u8string_view {
        const auto end_of_potential_cache = [&] {
            auto temp = self.current_pos;
            std::ranges::advance(temp, 2, self.source_end);
            return temp;
        }();
        return std::u8string_view{ self.cache.start_pos, end_of_potential_cache };
    }

    constexpr void tokenize_cache(const token_type type) {
        tokens.push_back(
            { type, source_ownership, { cache.start_pos, current_pos }, cache.row, cache.column });
    }

    [[nodiscard]] constexpr tokenize_state(source_code& source)
        : current_pos{ source.sv().begin() },
          source_end{ source.sv().end() },
          source_ownership{ source.get_ownership_of_code() },
          cache{ current_row, current_column, current_pos } {}

    // Advances until current_pos is at \n or at last character of source.
    constexpr void skip_line() {
        const auto new_line_pos     = std::ranges::find(current_pos, source_end, u8'\n');
        auto lenght_of_rest_of_line = static_cast<std::size_t>(new_line_pos - current_pos);

        // At last line, so advance only to last character.
        if (new_line_pos == source_end) lenght_of_rest_of_line -= 1;

        for ([[maybe_unused]] auto _ : std::views::iota(0uz, lenght_of_rest_of_line)) advance();
    }

    // Tests if \p str begins at current_pos.
    template<std::size_t wildcards_at_end = 0>
    [[nodiscard]] constexpr bool match_str(const std::u8string_view str) const {
        // Example:
        // p := current_pos and str.size() == 2
        //
        // ..., p - 1, p + 0, p + 1, p + 2, ...
        //           |str[0]|str[1]|
        //                         | p + str.size()
        //
        // So adding str.size() to current_pos will find one past the end of possible match.
        const auto one_past_end_of_possible_match = current_pos + str.size() + wildcards_at_end;
        const auto fits                           = one_past_end_of_possible_match <= source_end;
        if (not fits) return false;

        return std::ranges::equal(str, std::u8string_view{ current_pos, str.size() });
    }

    constexpr void advance() {
        ++current_pos;

        if (*current_pos == u8'\n') {
            ++current_row;
            current_column = 0;
        } else
            ++current_column;
    }
};

static constexpr const auto all_possible_tokens = [] {
    using namespace std::literals;
    return std::array{ // 3 char
                       u8"<<="sv,
                       u8">>="sv,
                       u8"<=>"sv,
                       // 2 char
                       u8"&="sv,
                       u8"^="sv,
                       u8"|="sv,
                       u8"->"sv,
                       u8"++"sv,
                       u8"--"sv,
                       u8"||"sv,
                       u8"&&"sv,
                       u8"+="sv,
                       u8"-="sv,
                       u8"*="sv,
                       u8"/="sv,
                       u8"%="sv,
                       u8"<<"sv,
                       u8">>"sv,
                       u8"<="sv,
                       u8">="sv,
                       u8"=="sv,
                       u8"!="sv,
                       // 1 char
                       u8"!"sv,
                       u8"#"sv,
                       u8"%"sv,
                       u8"&"sv,
                       u8"+"sv,
                       u8"-"sv,
                       u8"/"sv,
                       u8"<"sv,
                       u8"="sv,
                       u8">"sv,
                       u8"?"sv,
                       u8"\\"sv,
                       u8"^"sv,
                       u8"|"sv,
                       u8"~"sv,
                       u8"$"sv,
                       u8"@"sv
    };
}();

static constexpr auto longest_operator =
    std::ranges::max(all_possible_tokens, {}, [](const auto sv) { return sv.size(); });

[[nodiscard]] constexpr auto tokenize(source_code& source) -> std::vector<token> {
    // Construct a state pattern matcher:
    using namespace state_pattern_matcher;

    struct line_comment_t {
        auto operator()(predicate_tag, const tokenize_state& state) {
            return state.match_str(u8"//");
        }
        auto operator()(begin_tag, tokenize_state& state) {
            // Skip until at \n or at the last character of source.
            state.skip_line();
        }
        auto operator()(continuation_tag, tokenize_state& state) {
            // Skip until at \n or at the last character of source.
            state.skip_line();

            //    const auto at_last_char_of_last_line = not state.match_str<2>(u8"");
            //    if (last_line) state.advance();
        }

        auto operator()(end_tag, tokenize_state&) {}
    };

    struct block_comment_t {
        bool in_block_comment = false;
        bool delimiter_found  = false;
        bool second_skip      = false;

        auto operator()(predicate_tag, const tokenize_state& state) {
            if (delimiter_found) {
                // at / of delimiter */.
                if (second_skip) return false;
                // at * of delimiter */, so have to skip one more.
                second_skip = true;
                return true;
            }

            // If inside block comment predicate is always true.
            if (in_block_comment) {
                const auto at_end_delimiter = state.match_str(u8"*/");
                // Store if at delimiter, so next predicate can return false.
                delimiter_found = at_end_delimiter;
                return true;
            }

            // Looking for block comment.
            in_block_comment = state.match_str(u8"/*");
            return in_block_comment;
        }
        auto operator()(begin_tag, tokenize_state&) {}
        auto operator()(continuation_tag, tokenize_state&) {}
        auto operator()(end_tag, tokenize_state&) {
            in_block_comment = false;
            delimiter_found  = false;
            second_skip      = false;
        }
    };

    auto whitespace =
        sstd::overloaded{ [](predicate_tag, const auto& state) {
                             return classify_char(*state.current_pos) == char_class::whitespace;
                         },
                          [](begin_tag, auto& state) { state.set_cache(); },
                          [](continuation_tag, auto) {},
                          [](end_tag, auto& state) {
                              state.tokenize_cache(token_type::whitespace);
                          } };

    auto integer =
        sstd::overloaded{ [](predicate_tag, const auto& state) {
                             return classify_char(*state.current_pos) == char_class::integer;
                         },
                          [](begin_tag, auto& state) { state.set_cache(); },
                          [](continuation_tag, auto) {},
                          [](end_tag, auto& state) { state.tokenize_cache(token_type::integer); } };

    struct literal_t {
        std::optional<char8_t> delimiter{};
        bool delimiter_found = false;

        auto operator()(predicate_tag, const tokenize_state& state) {
            // Stop if delimiter is found on previous round.
            if (delimiter_found) return false;

            // If inside literal or at delimiter always continue.
            if (delimiter) {
                const auto at_end_delimiter = state.match_str(std::u8string{ delimiter.value() });
                // Store if at delimiter, so next predicate can return false.
                delimiter_found = at_end_delimiter;
                return true;
            }

            // Looking for new literal.
            const auto c_class = classify_char(*state.current_pos);
            return c_class == char_class::literal_scope_operator;
        }
        auto operator()(begin_tag, tokenize_state& state) {
            // Store which delimiter we are looking for.
            delimiter = *state.current_pos;
            state.set_cache();
        }
        auto operator()(continuation_tag, tokenize_state& state) {
            // Escape next char after backslash if it exits.
            if (state.match_str<1>(std::u8string{ '\\' })) state.advance();
        }

        auto operator()(end_tag, tokenize_state& state) {
            state.tokenize_cache(token_type::literal);
            delimiter.reset();
            delimiter_found = false;
        }
    };

    struct semantic_scope_operator_t {
        bool next_perdicate_is_false = false;
        auto operator()(predicate_tag, const tokenize_state& state) -> bool {
            if (next_perdicate_is_false) return false;
            const auto c_class = classify_char(*state.current_pos);
            return c_class == char_class::semantic_scope_operator;
        }
        auto operator()(begin_tag, tokenize_state& state) {
            next_perdicate_is_false = true;
            state.set_cache();
        }
        auto operator()(continuation_tag, tokenize_state&) {}

        auto operator()(end_tag, tokenize_state& state) {
            next_perdicate_is_false = false;
            state.tokenize_cache(token_type::semantic_scope_operator);
        }
    };

    struct operator_token_t {
        bool next_perdicate_is_false = false;

        void set_next_perdicate_is_false(const tokenize_state& state) {
            const auto next_cache_would_be_valid =
                state.potential_next_cache_as_sv().size() == 1
                or std::ranges::contains(all_possible_tokens, state.potential_next_cache_as_sv());

            const auto want_to_munch_more =
                state.cache_as_sv().size() < std::ranges::size(longest_operator);

            next_perdicate_is_false = not(want_to_munch_more and next_cache_would_be_valid);
        }

        auto operator()(predicate_tag, const tokenize_state& state) -> bool {
            if (next_perdicate_is_false) return false;

            return classify_char(*state.current_pos) == char_class::operator_unit;
        }
        auto operator()(begin_tag, tokenize_state& state) {
            state.set_cache();

            set_next_perdicate_is_false(state);
        }
        auto operator()(continuation_tag, tokenize_state& state) {
            set_next_perdicate_is_false(state);
        }

        auto operator()(end_tag, tokenize_state& state) {
            next_perdicate_is_false = false;
            state.tokenize_cache(token_type::operator_token);
        }
    };

    struct identifier_t {
        bool next_char_is_first = true;
        auto operator()(predicate_tag, const tokenize_state& state) {
            const auto char_class = classify_char(*state.current_pos);
            if (next_char_is_first) return char_class == char_class::id;
            return char_class == char_class::id or char_class == char_class::integer;
        }
        auto operator()(begin_tag, tokenize_state& state) {
            next_char_is_first = false;
            state.set_cache();
        }
        auto operator()(continuation_tag, tokenize_state) {}
        auto operator()(end_tag, tokenize_state& state) {
            next_char_is_first = true;
            state.tokenize_cache(token_type::identifier);
        }
    };

    struct error_token_t {
        bool next_perdicate_is_false = false;
        auto operator()(predicate_tag, const tokenize_state& state) -> bool {
            if (next_perdicate_is_false) return false;
            const auto c_class = classify_char(*state.current_pos);
            return c_class == char_class::other;
        }
        auto operator()(begin_tag, tokenize_state& state) {
            next_perdicate_is_false = true;
            state.set_cache();
        }
        auto operator()(continuation_tag, tokenize_state&) {}

        auto operator()(end_tag, tokenize_state& state) {
            next_perdicate_is_false = false;
            state.tokenize_cache(token_type::error);
        }
    };

    auto matcher = create_matcher_for<tokenize_state>(block_comment_t{},
                                                      line_comment_t{},
                                                      whitespace,
                                                      integer,
                                                      literal_t{},
                                                      semantic_scope_operator_t{},
                                                      operator_token_t{},
                                                      identifier_t{},
                                                      error_token_t{});

    const auto final_state =
        matcher({ source }, [](auto& state) { return state.current_pos == state.source_end; });

    return final_state.tokens;
}
} // namespace hycc
