#pragma once

/// @file Includes the implementation of the parser.

#include <algorithm>
#include <exception>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <tuple>
#include <vector>

#include "hycc/tokenizer.hpp"

namespace hycc {

struct token_pattern {
    token_type type;
    std::u8string_view sv;

    [[nodiscard]] constexpr auto match(const token& t) const {
        return type == t.type and sv == t.sv_in_source;
    }
    [[nodiscard]] friend constexpr bool operator==(const token_pattern&,
                                                   const token_pattern&) = default;
};

class parser_t {
    std::span<token const> tokens_;
    std::size_t next_unparsed_index_{ 0 };

    [[nodiscard]] constexpr auto tokens_left() { return tokens_.size() - next_unparsed_index_; }
    [[nodiscard]] constexpr decltype(auto) get_unparsed_tokens() {
        return tokens_.subspan(next_unparsed_index_);
    }

  public:
    [[nodiscard]] parser_t(const std::span<token const> tokens) : tokens_{ tokens } {}

    /// Ignores whitespace.
    [[nodiscard]] constexpr bool all_parsed() noexcept {
        namespace rv = std::ranges::views;
        return std::ranges::empty(get_unparsed_tokens() | rv::filter([](const token& t) {
                                      return not(t.type == token_type::whitespace);
                                  }));
    }
    [[nodiscard]] constexpr auto match_and_consume(const std::span<token_pattern const> pattern,
                                                   const bool skip_whitespace = true)
        -> std::optional<std::vector<token>> {
        if (pattern.empty()) return std::vector<token>{};

        auto skip = [&](const token& t) {
            return skip_whitespace and t.type == token_type::whitespace;
        };

        namespace rv = std::ranges::views;
        auto potential_matches_with_indecies =
            get_unparsed_tokens() | rv::enumerate | rv::filter([&](const auto& x) {
                const auto& [_, t] = x;
                if (skip(t)) return false;
                return true;
            });

        auto matched_tokens_with_indecies = rv::zip(pattern, potential_matches_with_indecies)
                                            | rv::filter([](const auto& x) {
                                                  const auto& [p, i_t] = x;
                                                  const auto& [_, t]   = i_t;
                                                  return p.match(t);
                                              })
                                            | rv::transform([](const auto& x) {
                                                  const auto& [_, i_t] = x;
                                                  return i_t;
                                              });

        using fold_t = std::tuple<std::vector<token>,
                                  std::ranges::range_difference_t<decltype(get_unparsed_tokens())>>;
        // Appends matched tokens together and take index of x unconditionally,
        // so afther the fold it will contain the last matched index.
        auto fold_f = [](fold_t lhs, const auto& x) {
            const auto& [i, t] = x;
            std::get<0>(lhs).push_back(t);
            std::get<1>(lhs) = i;
            return lhs;
        };
        auto [matched_tokens, last_index] =
            std::ranges::fold_left(matched_tokens_with_indecies, fold_t{}, fold_f);

        if (matched_tokens.size() == pattern.size()) {
            // All matched, so they can be consumed!
            next_unparsed_index_ += static_cast<std::size_t>(last_index) + 1;
            return matched_tokens;
        }
        return {};
    }
};

} // namespace hycc
