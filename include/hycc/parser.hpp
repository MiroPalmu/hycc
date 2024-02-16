#pragma once

/// @file Includes the implementation of the parser.

#include <algorithm>
#include <concepts>
#include <exception>
#include <format>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <tuple>
#include <vector>

#include "hycc/tokenizer.hpp"

namespace hycc {

class syntax_error : std::exception {
    std::string what_;

  public:
    [[nodiscard]] syntax_error(const token& t) {
        const auto sv_str = std::string{ t.sv_in_source.begin(), t.sv_in_source.end() };
        what_             = std::format("syntax error at [{}:{}]: {}", t.row, t.column, sv_str);
    }
    [[nodiscard]] syntax_error() : what_{ "syntax error at end of file" } {}

    const char* what() const noexcept { return what_.c_str(); }
};

struct token_pattern {
    token_type type;
    std::u8string_view sv;

    [[nodiscard]] constexpr auto match(const token& t) const {
        return type == t.type and sv == t.sv_in_source;
    }
    [[nodiscard]] friend constexpr bool operator==(const token_pattern&,
                                                   const token_pattern&) = default;
};

template<typename T>
concept token_matchable = std::same_as<T, token_pattern> or std::same_as<T, token_type>;

class parser_t {
    std::span<token const> tokens_;
    std::size_t next_unparsed_index_{ 0 };

    [[nodiscard]] constexpr auto tokens_left() { return tokens_.size() - next_unparsed_index_; }
    [[nodiscard]] constexpr decltype(auto) get_unparsed_tokens() {
        return tokens_.subspan(next_unparsed_index_);
    }

    [[nodiscard]] static constexpr bool match_pattern(const token_matchable auto p,
                                                      const token& t) {
        if constexpr (std::same_as<std::remove_cvref_t<decltype(p)>, token_type>) {
            return p == t.type;
        } else {
            return p.match(t);
        }

        static_assert(true, "This should not happen, due to token_matchable constraint :)");
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

    /// Helper function to deduce the template argument for the span version.
    template<std::ranges::range R>
        requires token_matchable<std::ranges::range_value_t<R>> and requires(R&& r) {
            std::span<std::ranges::range_value_t<R> const>{ std::forward<R>(r) };
        }
    [[nodiscard]] constexpr decltype(auto) match_and_consume(R&& pattern,
                                                             const bool skip_whitespace = true) {
        return match_and_consume(
            std::span<std::ranges::range_value_t<R> const>{ std::forward<R>(pattern) },
            skip_whitespace);
    }

    template<token_matchable T>
    [[nodiscard]] constexpr auto match_and_consume(const std::span<T const> pattern,
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
                                                  return match_pattern(p, t);
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

    using matched_type =
        decltype(std::declval<parser_t>().match_and_consume(std::span<token_type const>{}, false));

    /// Consumes until pattern is matched. Matched token is not included in return value but is parsed.
    [[nodiscard]] constexpr auto consume_until(const token_matchable auto pattern,
                                               const bool skip_whitespace = true) -> matched_type {
        auto skip = [&](const token& t) {
            return skip_whitespace and t.type == token_type::whitespace;
        };

        namespace rv                      = std::ranges::views;
        auto matched_tokens_with_indecies = get_unparsed_tokens() | rv::enumerate
                                            | rv::filter([&](const auto& x) {
                                                  const auto& [_, t] = x;
                                                  if (skip(t)) return false;
                                                  return true;
                                              })
                                            | rv::take_while([&](const auto& x) {
                                                  return not match_pattern(pattern, std::get<1>(x));
                                              });

        if (matched_tokens_with_indecies.empty()) {
            // Nothing matched
            if (match_and_consume(std::vector{ pattern }, skip_whitespace)) {
                // Because pattern was first that matched.
                return matched_type::value_type{};
            }

            return {};
        }
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

        if (last_index + 1 == std::ranges::ssize(tokens_)) {
            // Everything was matched, i.e. pattern was not present.
            return {};
        }

        // + 1 from indexing starting at 0.
        // + 1 from consuming the matched pattern.
        next_unparsed_index_ += static_cast<std::size_t>(last_index) + 1 + 1;

        return matched_tokens;
    }

    constexpr void throw_syntax_error(this auto&& self) {
        if (self.tokens_left()) throw syntax_error{ self.get_unparsed_tokens().front() };
        else
            throw syntax_error{};
    }
};

} // namespace hycc
