#include <boost/ut.hpp> // import boost.ut;

#include <algorithm>
#include <concepts>
#include <format>
#include <ranges>
#include <source_location>
#include <span>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include "hycc/parser.hpp"
#include "hycc/tokenizer.hpp"

void expect_token(const hycc::token_pattern& expected,
                  const hycc::token& got,
                  const std::source_location loc = std::source_location::current()) {
    const auto loc_info     = std::format("[actually at: {}]\n\n\t", loc.line());
    const auto error_end    = std::format("\n");
    const auto expected_str = std::string{ expected.sv.begin(), expected.sv.end() };
    const auto got_str      = std::string{ got.sv_in_source.begin(), got.sv_in_source.end() };

    boost::ut::expect(expected.type == got.type)
        << loc_info
        << std::format("expected hycc::token_type {}, got {} for: {}",
                       hycc::get_name(expected.type),
                       hycc::get_name(got.type),
                       got_str)
        << error_end;

    boost::ut::expect(expected.sv == got.sv_in_source)
        << loc_info << std::format("expected: {}, got {}", expected_str, got_str) << error_end;
}

void expect_token(const hycc::token_type expected,
                  const hycc::token& got,
                  const std::source_location loc = std::source_location::current()) {
    const auto loc_info  = std::format("[actually at: {}]\n\n\t", loc.line());
    const auto error_end = std::format("\n");
    const auto got_str   = std::string{ got.sv_in_source.begin(), got.sv_in_source.end() };

    boost::ut::expect(expected == got.type)
        << loc_info
        << std::format("expected hycc::token_type {}, got {} for: {}",
                       hycc::get_name(expected),
                       hycc::get_name(got.type),
                       got_str)
        << error_end;
}

template<hycc::token_matchable T>
void expect_tokens(const std::span<T const> expected,
                   const std::span<hycc::token const> got,
                   const std::source_location loc = std::source_location::current()) {
    const auto got_str = std::ranges::fold_left_first(
        got | std::views::transform([](const hycc::token& x) {
            return std::string{ x.sv_in_source.begin(), x.sv_in_source.end() };
        }),
        [](std::string lhs, std::string rhs) { return lhs += "\n" + rhs; });

    boost::ut::expect(expected.size() == got.size())
        << std::format("[actually at: {}]\n\n\t", loc.line())
        << std::format("expected {} tokens, got {}:\n{}",
                       expected.size(),
                       got.size(),
                       got_str.value_or("[no tokens]"));

    std::ranges::for_each(std::views::zip(expected, got),
                          [](const auto& x) { expect_token(std::get<0>(x), std::get<1>(x)); });
}

/// Helper to deduce type for the span version.
template<std::ranges::range R>
    requires hycc::token_matchable<std::ranges::range_value_t<R>> and requires(R&& r) {
        std::span<std::ranges::range_value_t<R> const>{ std::forward<R>(r) };
    }
void expect_tokens(R&& expected,
                   const std::span<hycc::token const> got,
                   const std::source_location loc = std::source_location::current()) {
    expect_tokens(std::span<std::ranges::range_value_t<R> const>{ std::forward<R>(expected) },
                  got,
                  loc);
}

int main() {
    using namespace boost::ut;
    using namespace hycc;

    "parser_t can be constructed"_test = [] {
        auto source       = source_code{ u8"123" };
        const auto tokens = tokenize(source);
        expect(nothrow([&] { [[maybe_unused]] auto _ = parser_t{ tokens }; }));
    };

    "parser_t can match patterns"_test = [] {
        auto source        = source_code{ u8"123" };
        const auto tokens  = tokenize(source);
        auto parser        = parser_t{ tokens };
        const auto pattern = std::vector<token_pattern>{ { token_type::integer, u8"123" } };
        const auto matched = parser.match_and_consume(pattern);
        expect(matched.has_value());
        expect_tokens(pattern, matched.value());
    };

    "parser_t consumes matched tokens"_test = [] {
        auto source         = source_code{ u8"123" };
        const auto tokens   = tokenize(source);
        auto parser         = parser_t{ tokens };
        const auto pattern  = std::vector<token_pattern>{ { token_type::integer, u8"123" } };
        const auto matched1 = parser.match_and_consume(pattern);
        expect(matched1.has_value());
        expect_tokens(pattern, matched1.value());
        const auto matched2 = parser.match_and_consume(pattern);
        expect(not matched2.has_value());
        expect(parser.all_parsed());
    };

    "parser_t can match patterns ignoring whitespace"_test = [] {
        auto source        = source_code{ u8" 123 " };
        const auto tokens  = tokenize(source);
        auto parser        = parser_t{ tokens };
        const auto pattern = std::vector<token_pattern>{ { token_type::integer, u8"123" } };
        const auto matched = parser.match_and_consume(pattern);
        expect(matched.has_value());
        expect_tokens(pattern, matched.value());
    };

    "parser_t can match whitespace"_test = [] {
        auto source        = source_code{ u8" 123 " };
        const auto tokens  = tokenize(source);
        auto parser        = parser_t{ tokens };
        const auto pattern = std::vector<token_pattern>{ { token_type::whitespace, u8" " },
                                                         { token_type::integer, u8"123" },
                                                         { token_type::whitespace, u8" " }

        };
        const auto matched = parser.match_and_consume(pattern, false);
        expect(matched.has_value());
        expect_tokens(pattern, matched.value());
    };

    "parser_t can match all token types"_test = [] {
        auto source        = source_code{ u8" 1``;+id" };
        const auto tokens  = tokenize(source);
        auto parser        = parser_t{ tokens };
        const auto pattern = std::vector<token_pattern>{
            { token_type::whitespace, u8" " },     { token_type::integer, u8"1" },
            { token_type::literal, u8"``" },       { token_type::semantic_scope_operator, u8";" },
            { token_type::operator_token, u8"+" }, { token_type::identifier, u8"id" }

        };
        const auto matched = parser.match_and_consume(pattern, false);
        expect(matched.has_value());
        expect_tokens(pattern, matched.value());
    };

    "parser_t can match all token types in str littered with whitespace"_test = [] {
        auto source       = source_code{ u8" 1\n``\t ; +\tid " };
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        const auto pattern =
            std::vector<token_pattern>{ { token_type::integer, u8"1" },
                                        { token_type::literal, u8"``" },
                                        { token_type::semantic_scope_operator, u8";" },
                                        { token_type::operator_token, u8"+" },
                                        { token_type::identifier, u8"id" }

            };
        const auto matched = parser.match_and_consume(pattern);
        expect(matched.has_value());
        expect_tokens(pattern, matched.value());
    };

    "parser_t matching can fail gracefully"_test = [] {
        auto source        = source_code{ u8"123" };
        const auto tokens  = tokenize(source);
        auto parser        = parser_t{ tokens };
        const auto pattern = std::vector<token_pattern>{ { token_type::literal, u8"123" } };
        const auto matched = parser.match_and_consume(pattern);
        expect(not matched.has_value());
        expect(not parser.all_parsed());
    };

    "parser_t matching can fail gracefully (more complex)"_test = [] {
        auto source       = source_code{ u8" 1\n``\t ; +\tid " };
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        const auto pattern =
            std::vector<token_pattern>{ { token_type::integer, u8"1" },
                                        { token_type::literal, u8"``" },
                                        { token_type::semantic_scope_operator, u8"(" },
                                        { token_type::operator_token, u8"+" },
                                        { token_type::identifier, u8"id" }

            };
        // Wrong semantic scope operator ( not ;.
        const auto matched = parser.match_and_consume(pattern);
        expect(not matched.has_value());
        expect(not parser.all_parsed());
    };

    "parser_t matching fail does not consume tokens"_test = [] {
        auto source         = source_code{ u8"123" };
        const auto tokens   = tokenize(source);
        auto parser         = parser_t{ tokens };
        const auto pattern1 = std::vector<token_pattern>{ { token_type::literal, u8"123" } };
        const auto matched1 = parser.match_and_consume(pattern1);
        expect(not matched1.has_value());
        expect(not parser.all_parsed());

        const auto pattern2 = std::vector<token_pattern>{ { token_type::integer, u8"123" } };
        const auto matched2 = parser.match_and_consume(pattern2);
        expect(matched2.has_value());
        expect_tokens(pattern2, matched2.value());
        expect(parser.all_parsed());

        const auto pattern3 = std::vector<token_pattern>{ { token_type::integer, u8"123" } };
        const auto matched3 = parser.match_and_consume(pattern3);
        expect(not matched3.has_value());
        expect(parser.all_parsed());
    };

    "parser_t matching fail does not consume tokens (more complex)"_test = [] {
        auto source       = source_code{ u8" 1\n``\t ; +\tid " };
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        const auto pattern1 =
            std::vector<token_pattern>{ { token_type::integer, u8"1" },
                                        { token_type::literal, u8"``" },
                                        { token_type::semantic_scope_operator, u8"(" },
                                        { token_type::operator_token, u8"+" },
                                        { token_type::identifier, u8"id" }

            };
        // Wrong semantic scope operator ( not ;.
        const auto matched1 = parser.match_and_consume(pattern1);
        expect(not matched1.has_value());

        const auto pattern2 =
            std::vector<token_pattern>{ { token_type::integer, u8"1" },
                                        { token_type::literal, u8"``" },
                                        { token_type::semantic_scope_operator, u8";" },
                                        { token_type::operator_token, u8"+" },
                                        { token_type::identifier, u8"id" }

            };
        const auto matched2 = parser.match_and_consume(pattern2);
        expect(matched2.has_value());
        expect_tokens(pattern2, matched2.value());

        const auto pattern3 =
            std::vector<token_pattern>{ { token_type::integer, u8"1" },
                                        { token_type::literal, u8"``" },
                                        { token_type::semantic_scope_operator, u8";" },
                                        { token_type::operator_token, u8"+" },
                                        { token_type::identifier, u8"id" }

            };
        const auto matched3 = parser.match_and_consume(pattern3);
        expect(not matched3.has_value());
    };

    "parser_t.all_parsed() ignores whitespace"_test = [] {
        auto source        = source_code{ u8"123  123  " };
        const auto tokens  = tokenize(source);
        auto parser        = parser_t{ tokens };
        const auto pattern = std::vector<token_pattern>{ { token_type::integer, u8"123" } };

        const auto matched1 = parser.match_and_consume(pattern);
        expect(matched1.has_value());
        expect_tokens(pattern, matched1.value());
        expect(not parser.all_parsed());

        const auto matched2 = parser.match_and_consume(pattern);
        expect(matched2.has_value());
        expect_tokens(pattern, matched2.value());
        expect(parser.all_parsed());
    };

    "parser_t can match type only patterns"_test = [] {
        auto source        = source_code{ u8"123" };
        const auto tokens  = tokenize(source);
        auto parser        = parser_t{ tokens };
        const auto pattern = std::vector{ token_type::integer };
        const auto matched = parser.match_and_consume(pattern);
        expect(matched.has_value());
        expect_tokens(pattern, matched.value());
    };

    "parser_t consumes matched tokens with type only patterns"_test = [] {
        auto source         = source_code{ u8"123" };
        const auto tokens   = tokenize(source);
        auto parser         = parser_t{ tokens };
        const auto pattern  = std::vector{ token_type::integer };
        const auto matched1 = parser.match_and_consume(pattern);
        expect(matched1.has_value());
        expect_tokens(pattern, matched1.value());
        const auto matched2 = parser.match_and_consume(pattern);
        expect(not matched2.has_value());
        expect(parser.all_parsed());
    };

    "parser_t can match type only patterns ignoring whitespace"_test = [] {
        auto source        = source_code{ u8" 123 " };
        const auto tokens  = tokenize(source);
        auto parser        = parser_t{ tokens };
        const auto pattern = std::vector{ token_type::integer };
        const auto matched = parser.match_and_consume(pattern);
        expect(matched.has_value());
        expect_tokens(pattern, matched.value());
    };

    "parser_t can match whitespace as type only"_test = [] {
        auto source       = source_code{ u8" 123 " };
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        const auto pattern =
            std::vector{ token_type::whitespace, token_type::integer, token_type::whitespace };
        const auto matched = parser.match_and_consume(pattern, false);
        expect(matched.has_value());
        expect_tokens(pattern, matched.value());
    };

    "parser_t can match all token types as type only"_test = [] {
        auto source       = source_code{ u8" 1``;+id" };
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        const auto pattern =
            std::vector{ token_type::whitespace,     token_type::integer,
                         token_type::literal,        token_type::semantic_scope_operator,
                         token_type::operator_token, token_type::identifier };
        const auto matched = parser.match_and_consume(pattern, false);
        expect(matched.has_value());
        expect_tokens(pattern, matched.value());
    };

    "parser_t can match all type only token patterns in str littered with whitespace"_test = [] {
        auto source        = source_code{ u8" 1\n``\t ; +\tid " };
        const auto tokens  = tokenize(source);
        auto parser        = parser_t{ tokens };
        const auto pattern = std::vector{ token_type::integer,
                                          token_type::literal,
                                          token_type::semantic_scope_operator,
                                          token_type::operator_token,
                                          token_type::identifier };
        const auto matched = parser.match_and_consume(pattern);
        expect(matched.has_value());
        expect_tokens(pattern, matched.value());
    };

    "parser_t matching can fail gracefully with type only patterns"_test = [] {
        auto source        = source_code{ u8"123" };
        const auto tokens  = tokenize(source);
        auto parser        = parser_t{ tokens };
        const auto pattern = std::vector{ token_type::literal };
        const auto matched = parser.match_and_consume(pattern);
        expect(not matched.has_value());
        expect(not parser.all_parsed());
    };

    "parser_t matching can fail gracefully with type only patterns (more complex)"_test = [] {
        auto source        = source_code{ u8" 1\n``\t ; +\tid " };
        const auto tokens  = tokenize(source);
        auto parser        = parser_t{ tokens };
        const auto pattern = std::vector{ token_type::integer,
                                          token_type::literal,
                                          token_type::literal,
                                          token_type::operator_token,
                                          token_type::identifier };

        const auto matched = parser.match_and_consume(pattern);
        expect(not matched.has_value());
        expect(not parser.all_parsed());
    };

    "parser_t type only matching fail does not consume tokens"_test = [] {
        auto source         = source_code{ u8"123" };
        const auto tokens   = tokenize(source);
        auto parser         = parser_t{ tokens };
        const auto pattern1 = std::vector{ token_type::literal };
        const auto matched1 = parser.match_and_consume(pattern1);
        expect(not matched1.has_value());
        expect(not parser.all_parsed());

        const auto pattern2 = std::vector{ token_type::integer };
        const auto matched2 = parser.match_and_consume(pattern2);
        expect(matched2.has_value());
        expect_tokens(pattern2, matched2.value());
        expect(parser.all_parsed());

        const auto pattern3 = std::vector{ token_type::integer };
        const auto matched3 = parser.match_and_consume(pattern3);
        expect(not matched3.has_value());
        expect(parser.all_parsed());
    };

    "parser_t type only matching fail does not consume tokens (more complex)"_test = [] {
        auto source         = source_code{ u8" 1\n``\t ; +\tid " };
        const auto tokens   = tokenize(source);
        auto parser         = parser_t{ tokens };
        const auto pattern1 = std::vector{ token_type::integer,
                                           token_type::literal,
                                           token_type::literal,
                                           token_type::operator_token,
                                           token_type::identifier };

        const auto matched1 = parser.match_and_consume(pattern1);
        expect(not matched1.has_value());

        const auto pattern2 = std::vector{ token_type::integer,
                                           token_type::literal,
                                           token_type::semantic_scope_operator,
                                           token_type::operator_token,
                                           token_type::identifier };

        const auto matched2 = parser.match_and_consume(pattern2);
        expect(matched2.has_value());
        expect_tokens(pattern2, matched2.value());

        const auto matched3 = parser.match_and_consume(pattern2);
        expect(not matched3.has_value());
    };

    "empty parser_t can match and consume pattern"_test = [] {
        auto source       = source_code{ u8"" };
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };

        const auto matched = parser.match_and_consume(std::vector{ token_type::identifier });

        expect(not matched.has_value());
    };

    "empty parser_t can consume until pattern"_test = [] {
        auto source       = source_code{ u8"" };
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };

        const auto matched = parser.consume_until(token_pattern{ token_type::identifier, u8"end" });

        expect(not matched.has_value());
    };

    "empty parser_t can consume until the first token"_test = [] {
        auto source       = source_code{ u8" end" };
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };

        const auto matched = parser.consume_until(token_pattern{ token_type::identifier, u8"end" });

        expect(matched.has_value());
        expect(matched.value().empty());
    };

    "parser_t can consume until pattern"_test = [] {
        auto source       = source_code{ u8"123abc +-end end" };
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };

        const auto matched = parser.consume_until(token_pattern{ token_type::identifier, u8"end" });
        const auto expected = std::vector{ token_pattern{ token_type::integer, u8"123" },
                                           token_pattern{ token_type::identifier, u8"abc" },
                                           token_pattern{ token_type::operator_token, u8"+" },
                                           token_pattern{ token_type::operator_token, u8"-" } };

        expect(matched.has_value());
        expect_tokens(expected, matched.value());

        expect(parser
                   .match_and_consume(
                       std::vector<token_pattern>{ { token_type::identifier, u8"end" } })
                   .has_value());
        expect(parser.all_parsed());
    };

    "parser_t can consume until token_type"_test = [] {
        auto source       = source_code{ u8"123 321 +-end end" };
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };

        const auto matched  = parser.consume_until(token_type::identifier);
        const auto expected = std::vector{ token_pattern{ token_type::integer, u8"123" },
                                           token_pattern{ token_type::integer, u8"321" },
                                           token_pattern{ token_type::operator_token, u8"+" },
                                           token_pattern{ token_type::operator_token, u8"-" } };

        expect(matched.has_value());
        expect_tokens(expected, matched.value());

        expect(parser
                   .match_and_consume(
                       std::vector<token_pattern>{ { token_type::identifier, u8"end" } })
                   .has_value());
        expect(parser.all_parsed());
    };

    "parser_t can consume until pattern with whitespace"_test = [] {
        auto source       = source_code{ u8"123abc +-end end" };
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };

        const auto matched =
            parser.consume_until(token_pattern{ token_type::identifier, u8"end" }, false);
        const auto expected = std::vector{ token_pattern{ token_type::integer, u8"123" },
                                           token_pattern{ token_type::identifier, u8"abc" },
                                           token_pattern{ token_type::whitespace, u8" " },
                                           token_pattern{ token_type::operator_token, u8"+" },
                                           token_pattern{ token_type::operator_token, u8"-" } };

        expect(matched.has_value());
        expect_tokens(expected, matched.value());

        expect(parser
                   .match_and_consume(
                       std::vector<token_pattern>{ { token_type::whitespace, u8" " },
                                                   { token_type::identifier, u8"end" } },
                       false)
                   .has_value());
        expect(parser.all_parsed());
    };
}
