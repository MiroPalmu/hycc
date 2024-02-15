#include <boost/ut.hpp> // import boost.ut;

#include <algorithm>
#include <array>
#include <cstddef>
#include <format>
#include <ranges>
#include <source_location>
#include <string>
#include <string_view>
#include <tuple>
#include <variant>
#include <vector>

#include "hycc/tokenizer.hpp"

struct mock_token {
    hycc::token_type type;
    std::u8string str;
    std::size_t row;
    std::size_t column;
};

void expect_token(const mock_token& expected,
                  const hycc::token& got,
                  const std::source_location loc = std::source_location::current()) {
    using namespace boost::ut;

    const auto loc_info     = std::format("[actually at: {}]\n\n\t", loc.line());
    const auto error_end    = std::format("\n");
    const auto expected_str = std::string{ expected.str.begin(), expected.str.end() };
    const auto got_str      = std::string{ got.sv_in_source.begin(), got.sv_in_source.end() };

    expect(expected.type == got.type) << loc_info
                                      << std::format("expected hycc::token_type {}, got {} for: {}",
                                                     hycc::get_name(expected.type),
                                                     hycc::get_name(got.type),
                                                     got_str)
                                      << error_end;

    expect(expected.str == got.sv_in_source)
        << loc_info << std::format("expected: {}, got {}", expected_str, got_str) << error_end;

    expect(expected.row == got.row)
        << loc_info << std::format("expected row {}, got row {}", expected.row, got.row)
        << error_end;
    expect(expected.column == got.column)
        << loc_info << std::format("expected column {}, got column {}", expected.column, got.column)
        << error_end;
}

int main() {
    using namespace boost::ut;

    "classified chars are regonized"_test = [] {
        struct char_class_pair {
            char8_t c;
            hycc::char_class c_class;
        };
        using namespace hycc;
        const auto correct_chars_and_classes =
            std::vector<char_class_pair>{ { 0x0009, char_class::whitespace },
                                          { 0x000B, char_class::whitespace },
                                          { 0x000C, char_class::whitespace },
                                          { 0x0020, char_class::whitespace },
                                          { 0x000A, char_class::whitespace },
                                          { 0x0021, char_class::operator_unit },
                                          { 0x0022, char_class::literal_scope_operator },
                                          { 0x0023, char_class::operator_unit },
                                          { 0x0025, char_class::operator_unit },
                                          { 0x0026, char_class::operator_unit },
                                          { 0x0027, char_class::literal_scope_operator },
                                          { 0x0028, char_class::semantic_scope_operator },
                                          { 0x0029, char_class::semantic_scope_operator },
                                          { 0x002A, char_class::operator_unit },
                                          { 0x002B, char_class::operator_unit },
                                          { 0x002C, char_class::semantic_scope_operator },
                                          { 0x002D, char_class::operator_unit },
                                          { 0x002E, char_class::semantic_scope_operator },
                                          { 0x002F, char_class::operator_unit },
                                          { 0x0030, char_class::integer },
                                          { 0x0031, char_class::integer },
                                          { 0x0032, char_class::integer },
                                          { 0x0033, char_class::integer },
                                          { 0x0034, char_class::integer },
                                          { 0x0035, char_class::integer },
                                          { 0x0036, char_class::integer },
                                          { 0x0037, char_class::integer },
                                          { 0x0038, char_class::integer },
                                          { 0x0039, char_class::integer },
                                          { 0x003A, char_class::semantic_scope_operator },
                                          { 0x003B, char_class::semantic_scope_operator },
                                          { 0x003C, char_class::operator_unit },
                                          { 0x003D, char_class::operator_unit },
                                          { 0x003E, char_class::operator_unit },
                                          { 0x003F, char_class::operator_unit },
                                          { 0x0041, char_class::id },
                                          { 0x0042, char_class::id },
                                          { 0x0043, char_class::id },
                                          { 0x0044, char_class::id },
                                          { 0x0045, char_class::id },
                                          { 0x0046, char_class::id },
                                          { 0x0047, char_class::id },
                                          { 0x0048, char_class::id },
                                          { 0x0049, char_class::id },
                                          { 0x004A, char_class::id },
                                          { 0x004B, char_class::id },
                                          { 0x004C, char_class::id },
                                          { 0x004D, char_class::id },
                                          { 0x004E, char_class::id },
                                          { 0x004F, char_class::id },
                                          { 0x0050, char_class::id },
                                          { 0x0051, char_class::id },
                                          { 0x0052, char_class::id },
                                          { 0x0054, char_class::id },
                                          { 0x0054, char_class::id },
                                          { 0x0055, char_class::id },
                                          { 0x0056, char_class::id },
                                          { 0x0057, char_class::id },
                                          { 0x0058, char_class::id },
                                          { 0x0059, char_class::id },
                                          { 0x005A, char_class::id },
                                          { 0x005B, char_class::semantic_scope_operator },
                                          { 0x005C, char_class::operator_unit },
                                          { 0x005D, char_class::semantic_scope_operator },
                                          { 0x005E, char_class::operator_unit },
                                          { 0x005F, char_class::id },
                                          { 0x0061, char_class::id },
                                          { 0x0062, char_class::id },
                                          { 0x0063, char_class::id },
                                          { 0x0064, char_class::id },
                                          { 0x0065, char_class::id },
                                          { 0x0066, char_class::id },
                                          { 0x0067, char_class::id },
                                          { 0x0068, char_class::id },
                                          { 0x0069, char_class::id },
                                          { 0x006A, char_class::id },
                                          { 0x006B, char_class::id },
                                          { 0x006C, char_class::id },
                                          { 0x006D, char_class::id },
                                          { 0x006E, char_class::id },
                                          { 0x006F, char_class::id },
                                          { 0x0070, char_class::id },
                                          { 0x0071, char_class::id },
                                          { 0x0072, char_class::id },
                                          { 0x0073, char_class::id },
                                          { 0x0074, char_class::id },
                                          { 0x0075, char_class::id },
                                          { 0x0076, char_class::id },
                                          { 0x0077, char_class::id },
                                          { 0x0078, char_class::id },
                                          { 0x0079, char_class::id },
                                          { 0x007A, char_class::id },
                                          { 0x007B, char_class::semantic_scope_operator },
                                          { 0x007C, char_class::operator_unit },
                                          { 0x007D, char_class::semantic_scope_operator },
                                          { 0x007E, char_class::operator_unit },
                                          { 0x0024, char_class::operator_unit },
                                          { 0x0040, char_class::operator_unit },
                                          { 0x0060, char_class::literal_scope_operator } };

        auto expect_correct = [&](const char_class_pair c) {
            expect(hycc::classify_char(c.c) == c.c_class)
                << std::format("{:X} is classified wrong!", c.c);
        };
        std::ranges::for_each(correct_chars_and_classes, expect_correct);
    };

    "some unclassified chars are classified correctly"_test = [] {
        const auto chars = std::vector<char8_t>{
            0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
            0x0008, 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016,
            0x0017, 0x0018, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
        };

        std::ranges::for_each(chars, [](const auto c) {
            using enum hycc::char_class;
            expect(hycc::classify_char(c) == other) << std::format("{:X} is classified wrong!", c);
        });
    };

    "whitespace tokens are tokenized"_test = [] {
        using namespace hycc;

        auto source_code1  = source_code{ std::u8string{ u8"a\nb c\t" } };
        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 6);
        expect_token({ token_type::identifier, u8"a", 0, 1 }, tokens1[0]);
        expect_token({ token_type::whitespace, u8"\n", 1, 0 }, tokens1[1]);
        expect_token({ token_type::identifier, u8"b", 1, 1 }, tokens1[2]);
        expect_token({ token_type::whitespace, u8" ", 1, 2 }, tokens1[3]);
        expect_token({ token_type::identifier, u8"c", 1, 3 }, tokens1[4]);
        expect_token({ token_type::whitespace, u8"\t", 1, 4 }, tokens1[5]);
    };

    "whitespace tokens are concatted"_test = [] {
        using namespace hycc;

        auto source_code1  = source_code{ std::u8string{ u8"a\n \tbc" } };
        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 3);
        expect_token({ token_type::identifier, u8"a", 0, 1 }, tokens1[0]);
        expect_token({ token_type::whitespace, u8"\n \t", 1, 0 }, tokens1[1]);
        expect_token({ token_type::identifier, u8"bc", 1, 3 }, tokens1[2]);

        auto source_code2  = hycc::source_code{ std::u8string{ u8"ab \n\tc" } };
        const auto tokens2 = hycc::tokenize(source_code2);
        expect(tokens2.size() == 3);
        expect_token({ token_type::identifier, u8"ab", 0, 1 }, tokens2[0]);
        expect_token({ token_type::whitespace, u8" \n\t", 0, 3 }, tokens2[1]);
        expect_token({ token_type::identifier, u8"c", 1, 2 }, tokens2[2]);
    };

    "integer tokens are regonized"_test = [] {
        using namespace hycc;

        auto source_code1  = hycc::source_code{ std::u8string{ u8"0 1 2 3 4 5 6 7 8 9 " } };
        const auto tokens1 = hycc::tokenize(source_code1);
        expect(tokens1.size() == 20);

        namespace sv = std::views;
        auto foo     = sv::zip(source_code1.sv(), tokens1) | sv::enumerate;
        for (const auto [i, token] : foo) {
            expect(std::get<1>(token).type
                   == ((i % 2) ? token_type::whitespace : token_type::integer));
            expect(std::get<1>(token).sv_in_source == std::u8string{ std::get<0>(token) });
            expect(std::get<1>(token).row == 0);
            expect(std::get<1>(token).column == static_cast<std::size_t>(i) + 1);
        }
    };

    "integer tokens are concatted"_test = [] {
        using namespace hycc;
        auto source_code1  = source_code{ std::u8string{ u8"123\nabc\n 45\n678" } };
        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 7);
        expect_token({ token_type::integer, u8"123", 0, 1 }, tokens1[0]);
        expect_token({ token_type::whitespace, u8"\n", 1, 0 }, tokens1[1]);
        expect_token({ token_type::identifier, u8"abc", 1, 1 }, tokens1[2]);
        expect_token({ token_type::whitespace, u8"\n ", 2, 0 }, tokens1[3]);
        expect_token({ token_type::integer, u8"45", 2, 2 }, tokens1[4]);
        expect_token({ token_type::whitespace, u8"\n", 3, 0 }, tokens1[5]);
        expect_token({ token_type::integer, u8"678", 3, 1 }, tokens1[6]);
    };

    "literal tokens are tokenized"_test = [] {
        using namespace hycc;

        auto source_code1 = source_code{ std::u8string{ u8"\"123\"_`abc`_'\n  '" } };

        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 5);
        expect_token({ token_type::literal, u8"\"123\"", 0, 1 }, tokens1[0]);
        expect_token({ token_type::identifier, u8"_", 0, 6 }, tokens1[1]);
        expect_token({ token_type::literal, u8"`abc`", 0, 7 }, tokens1[2]);
        expect_token({ token_type::identifier, u8"_", 0, 12 }, tokens1[3]);
        expect_token({ token_type::literal, u8"'\n  '", 0, 13 }, tokens1[4]);
    };

    "line breaks in literal tokens effect source position"_test = [] {
        using namespace hycc;

        auto source_code1 = source_code{ std::u8string{ u8"'\n\n\n\n  'abc" } };

        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 2);
        expect_token({ token_type::literal, u8"'\n\n\n\n  '", 0, 1 }, tokens1[0]);
        expect_token({ token_type::identifier, u8"abc", 4, 4 }, tokens1[1]);
    };

    "empty literal tokens are possible"_test = [] {
        using namespace hycc;

        auto source_code1 = source_code{ std::u8string{ u8"''x\"\"x``" } };

        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 5);
        expect_token({ token_type::literal, u8"''", 0, 1 }, tokens1[0]);
        expect_token({ token_type::identifier, u8"x", 0, 3 }, tokens1[1]);
        expect_token({ token_type::literal, u8"\"\"", 0, 4 }, tokens1[2]);
        expect_token({ token_type::identifier, u8"x", 0, 6 }, tokens1[3]);
        expect_token({ token_type::literal, u8"``", 0, 7 }, tokens1[4]);
    };

    "escaping characters in literal tokens is possible"_test = [] {
        using namespace hycc;

        auto source_code1 = source_code{ std::u8string{ u8R"('\'\''"\"\"")" } };

        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 2);
        expect_token({ token_type::literal, u8R"('\'\'')", 0, 1 }, tokens1[0]);
        expect_token({ token_type::literal, u8R"("\"\"")", 0, 7 }, tokens1[1]);
    };

    "source can stop in middle of literal token"_test = [] {
        using namespace hycc;

        auto source_code1 = source_code{ std::u8string{ u8R"('ab)" } };

        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 1);
        expect_token({ token_type::literal, u8R"('ab)", 0, 1 }, tokens1[0]);
    };

    "semantic scope operators are tokenized"_test = [] {
        using namespace hycc;

        auto source_code1 = source_code{ std::u8string{ u8"(),.:;[]{}" } };

        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 10);
        const auto corrects = std::u8string{ u8"(),.:;[]{}" };
        for (const auto [i_int, c] : corrects | std::views::enumerate) {
            const auto i = static_cast<std::size_t>(i_int);
            expect_token({ token_type::semantic_scope_operator, std::u8string{ c }, 0, i + 1 },
                         tokens1[i]);
        }

        auto source_code2 = source_code{ std::u8string{ u8"[(   )]" } };

        const auto tokens2 = tokenize(source_code2);
        expect(tokens2.size() == 5);

        expect_token({ token_type::semantic_scope_operator, std::u8string{ u8'[' }, 0, 1 },
                     tokens2[0]);
        expect_token({ token_type::semantic_scope_operator, std::u8string{ u8'(' }, 0, 2 },
                     tokens2[1]);
        expect_token({ token_type::whitespace, u8"   ", 0, 3 }, tokens2[2]);
        expect_token({ token_type::semantic_scope_operator, std::u8string{ u8')' }, 0, 6 },
                     tokens2[3]);
        expect_token({ token_type::semantic_scope_operator, std::u8string{ u8']' }, 0, 7 },
                     tokens2[4]);
    };

    "operators are tokenized"_test = [] {
        using namespace hycc;

        auto source_code1 = source_code{ std::u8string{ u8"! # % & + - & < = > ? \\ ^ | ~ $ @ " } };

        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 17 * 2);
        const auto corrects = std::u8string{ u8"!#%&+-&<=>?\\^|~$@" };
        for (const auto [i_s, c] : corrects | std::views::enumerate) {
            const auto i = static_cast<std::size_t>(i_s);

            const auto column_of_non_whitespace = 2 * i + 1;
            expect_token(
                { token_type::operator_token, std::u8string{ c }, 0, column_of_non_whitespace },
                tokens1[2 * i]);
            expect_token({ token_type::whitespace, u8" ", 0, column_of_non_whitespace + 1 },
                         tokens1[2 * i + 1]);
        }
    };

    "operators are not concatted"_test = [] {
        using namespace hycc;

        auto source_code1 = source_code{ std::u8string{ u8"!#%&+-&<=>?\\^|~$@" } };

        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 17);
        const auto corrects = std::u8string{ u8"!#%&+-&<=>?\\^|~$@" };
        for (const auto [i_s, c] : corrects | std::views::enumerate) {
            const auto i = static_cast<std::size_t>(i_s);

            expect_token({ token_type::operator_token, std::u8string{ c }, 0, i + 1 }, tokens1[i]);
        }
    };

    "idnentifier tokens are tokenized"_test = [] {
        using namespace hycc;
        auto source_code1  = source_code{ std::u8string{ u8"a b c" } };
        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 5);

        expect_token({ token_type::identifier, u8"a", 0, 1 }, tokens1[0]);
        expect_token({ token_type::whitespace, u8" ", 0, 2 }, tokens1[1]);
        expect_token({ token_type::identifier, u8"b", 0, 3 }, tokens1[2]);
        expect_token({ token_type::whitespace, u8" ", 0, 4 }, tokens1[3]);
        expect_token({ token_type::identifier, u8"c", 0, 5 }, tokens1[4]);
    };

    "idnentifier tokens are concatted"_test = [] {
        using namespace hycc;
        auto source_code1  = source_code{ std::u8string{ u8"abc def" } };
        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 3);

        expect_token({ token_type::identifier, u8"abc", 0, 1 }, tokens1[0]);
        expect_token({ token_type::whitespace, u8" ", 0, 4 }, tokens1[1]);
        expect_token({ token_type::identifier, u8"def", 0, 5 }, tokens1[2]);
    };

    "identifier can not begin with number"_test = [] {
        using namespace hycc;
        auto source_code1  = source_code{ std::u8string{ u8"0x0x x00x00" } };
        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 4);

        expect_token({ token_type::integer, u8"0", 0, 1 }, tokens1[0]);
        expect_token({ token_type::identifier, u8"x0x", 0, 2 }, tokens1[1]);
        expect_token({ token_type::whitespace, u8" ", 0, 5 }, tokens1[2]);
        expect_token({ token_type::identifier, u8"x00x00", 0, 6 }, tokens1[3]);
    };

    "line comments are skipped"_test = [] {
        using namespace hycc;
        auto source_code1  = source_code{ std::u8string{ u8R"(// line_zero
line_one // This is comment.
// So is this.
line_three)" } };
        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 3);

        // Newline at line 1 column 0.
        expect_token({ token_type::identifier, u8"line_one", 1, 1 }, tokens1[0]);
        expect_token({ token_type::whitespace, u8" ", 1, 9 }, tokens1[1]);
        expect_token({ token_type::identifier, u8"line_three", 3, 1 }, tokens1[2]);
    };

    "line comment can be last and first line of source"_test = [] {
        using namespace hycc;
        auto source_code1  = source_code{ std::u8string{ u8R"(// first line
// last line)" } };
        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 0);

        auto source_code2  = source_code{ std::u8string{ u8R"(// first line
foo
// last line)" } };
        const auto tokens2 = tokenize(source_code2);
        expect(tokens2.size() == 2);
        expect_token({ token_type::identifier, u8"foo", 1, 1 }, tokens2[0]);
        expect_token({ token_type::whitespace, u8"\n", 2, 0 }, tokens2[1]);

        auto source_code3  = source_code{ std::u8string{ u8R"(// first line
foo// middle
// last line)" } };
        const auto tokens3 = tokenize(source_code3);
        expect(tokens3.size() == 1);
        expect_token({ token_type::identifier, u8"foo", 1, 1 }, tokens3[0]);
    };

    "line comment can be empty"_test = [] {
        using namespace hycc;
        auto source_code1  = source_code{ std::u8string{ u8R"(//
//)" } };
        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 0);

        auto source_code2  = source_code{ std::u8string{ u8R"(//
foo
//)" } };
        const auto tokens2 = tokenize(source_code2);
        expect(tokens2.size() == 2);
        expect_token({ token_type::identifier, u8"foo", 1, 1 }, tokens2[0]);
        expect_token({ token_type::whitespace, u8"\n", 2, 0 }, tokens2[1]);

        auto source_code3 = source_code{ std::u8string{ u8R"(// first line
foo//
//)" } };
        //
        const auto tokens3 = tokenize(source_code3);
        expect(tokens3.size() == 1);
        expect_token({ token_type::identifier, u8"foo", 1, 1 }, tokens3[0]);
    };

    "block comments are skipped"_test = [] {
        using namespace hycc;
        auto source_code1  = source_code{ std::u8string{ u8R"(/* line_zero
*/line_one /* This is comment.
// So is this.
*/line_three)" } };
        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 3);

        // Newline at line 1 column 0.
        expect_token({ token_type::identifier, u8"line_one", 1, 3 }, tokens1[0]);
        expect_token({ token_type::whitespace, u8" ", 1, 11 }, tokens1[1]);
        expect_token({ token_type::identifier, u8"line_three", 3, 3 }, tokens1[2]);
    };

    "block comment can be on last and first line of source"_test = [] {
        using namespace hycc;
        auto source_code1  = source_code{ std::u8string{ u8R"(/*first line

last line*/)" } };
        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 0);

        auto source_code2  = source_code{ std::u8string{ u8R"(/* first line
*/foo/*
last line*/)" } };
        const auto tokens2 = tokenize(source_code2);
        expect(tokens2.size() == 1);
        expect_token({ token_type::identifier, u8"foo", 1, 3 }, tokens2[0]);

        auto source_code3  = source_code{ std::u8string{ u8R"(/* first line
*/foo/* middle line
   last line */)" } };
        const auto tokens3 = tokenize(source_code3);
        expect(tokens3.size() == 1);
        expect_token({ token_type::identifier, u8"foo", 1, 3 }, tokens3[0]);
    };

    "block comment can be empty"_test = [] {
        using namespace hycc;
        auto source_code1  = source_code{ std::u8string{ u8R"(/**//**//**/)" } };
        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 0);

        auto source_code2  = source_code{ std::u8string{ u8R"(/**/foo/**/)" } };
        const auto tokens2 = tokenize(source_code2);
        expect(tokens2.size() == 1);
        expect_token({ token_type::identifier, u8"foo", 0, 5 }, tokens2[0]);
    };

    "Error tokens are tokenized"_test = [] {
        using namespace hycc;

        const auto str     = std::array<char, 5>{ 0x01, 0x02, 0x03, 0x04, 0x05 };
        auto source_code1  = source_code{ std::u8string{ str.begin(), str.end() } };
        const auto tokens1 = tokenize(source_code1);

        expect(tokens1.size() == 5);
        for (const auto [i_s, c] : str | std::views::enumerate) {
            const auto i  = static_cast<std::size_t>(i_s);
            const auto c8 = static_cast<char8_t>(c);
            expect_token({ token_type::error, std::u8string{ c8 }, 0, i + 1 }, tokens1[i]);
        }
    };
}
