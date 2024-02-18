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
        expect_token({ token_type::identifier, u8"a", 1, 1 }, tokens1[0]);
        expect_token({ token_type::whitespace, u8"\n", 2, 0 }, tokens1[1]);
        expect_token({ token_type::identifier, u8"b", 2, 1 }, tokens1[2]);
        expect_token({ token_type::whitespace, u8" ", 2, 2 }, tokens1[3]);
        expect_token({ token_type::identifier, u8"c", 2, 3 }, tokens1[4]);
        expect_token({ token_type::whitespace, u8"\t", 2, 4 }, tokens1[5]);
    };

    "whitespace tokens are concatted"_test = [] {
        using namespace hycc;

        auto source_code1  = source_code{ std::u8string{ u8"a\n \tbc" } };
        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 3);
        expect_token({ token_type::identifier, u8"a", 1, 1 }, tokens1[0]);
        expect_token({ token_type::whitespace, u8"\n \t", 2, 0 }, tokens1[1]);
        expect_token({ token_type::identifier, u8"bc", 2, 3 }, tokens1[2]);

        auto source_code2  = hycc::source_code{ std::u8string{ u8"ab \n\tc" } };
        const auto tokens2 = hycc::tokenize(source_code2);
        expect(tokens2.size() == 3);
        expect_token({ token_type::identifier, u8"ab", 1, 1 }, tokens2[0]);
        expect_token({ token_type::whitespace, u8" \n\t", 1, 3 }, tokens2[1]);
        expect_token({ token_type::identifier, u8"c", 2, 2 }, tokens2[2]);
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
            expect(std::get<1>(token).row == 1);
            expect(std::get<1>(token).column == static_cast<std::size_t>(i) + 1);
        }
    };

    "integer tokens are concatted"_test = [] {
        using namespace hycc;
        auto source_code1  = source_code{ std::u8string{ u8"123\nabc\n 45\n678" } };
        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 7);
        expect_token({ token_type::integer, u8"123", 1, 1 }, tokens1[0]);
        expect_token({ token_type::whitespace, u8"\n", 2, 0 }, tokens1[1]);
        expect_token({ token_type::identifier, u8"abc", 2, 1 }, tokens1[2]);
        expect_token({ token_type::whitespace, u8"\n ", 3, 0 }, tokens1[3]);
        expect_token({ token_type::integer, u8"45", 3, 2 }, tokens1[4]);
        expect_token({ token_type::whitespace, u8"\n", 4, 0 }, tokens1[5]);
        expect_token({ token_type::integer, u8"678", 4, 1 }, tokens1[6]);
    };

    "literal tokens are tokenized"_test = [] {
        using namespace hycc;

        auto source_code1 = source_code{ std::u8string{ u8"\"123\"_`abc`_'\n  '" } };

        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 5);
        expect_token({ token_type::literal, u8"\"123\"", 1, 1 }, tokens1[0]);
        expect_token({ token_type::identifier, u8"_", 1, 6 }, tokens1[1]);
        expect_token({ token_type::literal, u8"`abc`", 1, 7 }, tokens1[2]);
        expect_token({ token_type::identifier, u8"_", 1, 12 }, tokens1[3]);
        expect_token({ token_type::literal, u8"'\n  '", 1, 13 }, tokens1[4]);
    };

    "line breaks in literal tokens effect source position"_test = [] {
        using namespace hycc;

        auto source_code1 = source_code{ std::u8string{ u8"'\n\n\n\n  'abc" } };

        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 2);
        expect_token({ token_type::literal, u8"'\n\n\n\n  '", 1, 1 }, tokens1[0]);
        expect_token({ token_type::identifier, u8"abc", 5, 4 }, tokens1[1]);
    };

    "empty literal tokens are possible"_test = [] {
        using namespace hycc;

        auto source_code1 = source_code{ std::u8string{ u8"''x\"\"x``" } };

        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 5);
        expect_token({ token_type::literal, u8"''", 1, 1 }, tokens1[0]);
        expect_token({ token_type::identifier, u8"x", 1, 3 }, tokens1[1]);
        expect_token({ token_type::literal, u8"\"\"", 1, 4 }, tokens1[2]);
        expect_token({ token_type::identifier, u8"x", 1, 6 }, tokens1[3]);
        expect_token({ token_type::literal, u8"``", 1, 7 }, tokens1[4]);
    };

    "escaping characters in literal tokens is possible"_test = [] {
        using namespace hycc;

        auto source_code1 = source_code{ std::u8string{ u8R"('\'\''"\"\"")" } };

        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 2);
        expect_token({ token_type::literal, u8R"('\'\'')", 1, 1 }, tokens1[0]);
        expect_token({ token_type::literal, u8R"("\"\"")", 1, 7 }, tokens1[1]);
    };

    "source can stop in middle of literal token"_test = [] {
        using namespace hycc;

        auto source_code1 = source_code{ std::u8string{ u8R"('ab)" } };

        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 1);
        expect_token({ token_type::literal, u8R"('ab)", 1, 1 }, tokens1[0]);
    };

    "semantic scope operators are tokenized"_test = [] {
        using namespace hycc;

        auto source_code1 = source_code{ std::u8string{ u8"(),.:;[]{}" } };

        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 10);
        const auto corrects = std::u8string{ u8"(),.:;[]{}" };
        for (const auto [i_int, c] : corrects | std::views::enumerate) {
            const auto i = static_cast<std::size_t>(i_int);
            expect_token({ token_type::semantic_scope_operator, std::u8string{ c }, 1, i + 1 },
                         tokens1[i]);
        }

        auto source_code2 = source_code{ std::u8string{ u8"[(   )]" } };

        const auto tokens2 = tokenize(source_code2);
        expect(tokens2.size() == 5);

        expect_token({ token_type::semantic_scope_operator, std::u8string{ u8'[' }, 1, 1 },
                     tokens2[0]);
        expect_token({ token_type::semantic_scope_operator, std::u8string{ u8'(' }, 1, 2 },
                     tokens2[1]);
        expect_token({ token_type::whitespace, u8"   ", 1, 3 }, tokens2[2]);
        expect_token({ token_type::semantic_scope_operator, std::u8string{ u8')' }, 1, 6 },
                     tokens2[3]);
        expect_token({ token_type::semantic_scope_operator, std::u8string{ u8']' }, 1, 7 },
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
                { token_type::operator_token, std::u8string{ c }, 1, column_of_non_whitespace },
                tokens1[2 * i]);
            expect_token({ token_type::whitespace, u8" ", 1, column_of_non_whitespace + 1 },
                         tokens1[2 * i + 1]);
        }
    };

    "operators are not concatted, but maximally munched"_test = [] {
        using namespace hycc;

        auto source_code1 = source_code{ std::u8string{ u8"!#%&+-&<=>?\\^|~$@" } };

        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 15);
        expect_token({ token_type::operator_token, u8"!", 1, 1 }, tokens1[0]);
        expect_token({ token_type::operator_token, u8"#", 1, 2 }, tokens1[1]);
        expect_token({ token_type::operator_token, u8"%", 1, 3 }, tokens1[2]);
        expect_token({ token_type::operator_token, u8"&", 1, 4 }, tokens1[3]);
        expect_token({ token_type::operator_token, u8"+", 1, 5 }, tokens1[4]);
        expect_token({ token_type::operator_token, u8"-", 1, 6 }, tokens1[5]);
        expect_token({ token_type::operator_token, u8"&", 1, 7 }, tokens1[6]);
        expect_token({ token_type::operator_token, u8"<=>", 1, 8 }, tokens1[7]);
        expect_token({ token_type::operator_token, u8"?", 1, 11 }, tokens1[8]);
        expect_token({ token_type::operator_token, u8"\\", 1, 12 }, tokens1[9]);
        expect_token({ token_type::operator_token, u8"^", 1, 13 }, tokens1[10]);
        expect_token({ token_type::operator_token, u8"|", 1, 14 }, tokens1[11]);
        expect_token({ token_type::operator_token, u8"~", 1, 15 }, tokens1[12]);
        expect_token({ token_type::operator_token, u8"$", 1, 16 }, tokens1[13]);
        expect_token({ token_type::operator_token, u8"@", 1, 17 }, tokens1[14]);
    };

    "idnentifier tokens are tokenized"_test = [] {
        using namespace hycc;
        auto source_code1  = source_code{ std::u8string{ u8"a b c" } };
        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 5);

        expect_token({ token_type::identifier, u8"a", 1, 1 }, tokens1[0]);
        expect_token({ token_type::whitespace, u8" ", 1, 2 }, tokens1[1]);
        expect_token({ token_type::identifier, u8"b", 1, 3 }, tokens1[2]);
        expect_token({ token_type::whitespace, u8" ", 1, 4 }, tokens1[3]);
        expect_token({ token_type::identifier, u8"c", 1, 5 }, tokens1[4]);
    };

    "idnentifier tokens are concatted"_test = [] {
        using namespace hycc;
        auto source_code1  = source_code{ std::u8string{ u8"abc def" } };
        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 3);

        expect_token({ token_type::identifier, u8"abc", 1, 1 }, tokens1[0]);
        expect_token({ token_type::whitespace, u8" ", 1, 4 }, tokens1[1]);
        expect_token({ token_type::identifier, u8"def", 1, 5 }, tokens1[2]);
    };

    "identifier can not begin with number"_test = [] {
        using namespace hycc;
        auto source_code1  = source_code{ std::u8string{ u8"0x0x x00x00" } };
        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 4);

        expect_token({ token_type::integer, u8"0", 1, 1 }, tokens1[0]);
        expect_token({ token_type::identifier, u8"x0x", 1, 2 }, tokens1[1]);
        expect_token({ token_type::whitespace, u8" ", 1, 5 }, tokens1[2]);
        expect_token({ token_type::identifier, u8"x00x00", 1, 6 }, tokens1[3]);
    };

    "line comments are skipped"_test = [] {
        using namespace hycc;
        auto source_code1  = source_code{ std::u8string{ u8R"(// line_one
line_two // This is comment.
// So is this.
line_four)" } };
        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 3);

        // Newline at line 1 column 0.
        expect_token({ token_type::identifier, u8"line_two", 2, 1 }, tokens1[0]);
        expect_token({ token_type::whitespace, u8" ", 2, 9 }, tokens1[1]);
        expect_token({ token_type::identifier, u8"line_four", 4, 1 }, tokens1[2]);
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
        expect_token({ token_type::identifier, u8"foo", 2, 1 }, tokens2[0]);
        expect_token({ token_type::whitespace, u8"\n", 3, 0 }, tokens2[1]);

        auto source_code3  = source_code{ std::u8string{ u8R"(// first line
foo// middle
// last line)" } };
        const auto tokens3 = tokenize(source_code3);
        expect(tokens3.size() == 1);
        expect_token({ token_type::identifier, u8"foo", 2, 1 }, tokens3[0]);
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
        expect_token({ token_type::identifier, u8"foo", 2, 1 }, tokens2[0]);
        expect_token({ token_type::whitespace, u8"\n", 3, 0 }, tokens2[1]);

        auto source_code3 = source_code{ std::u8string{ u8R"(// first line
foo//
//)" } };
        //
        const auto tokens3 = tokenize(source_code3);
        expect(tokens3.size() == 1);
        expect_token({ token_type::identifier, u8"foo", 2, 1 }, tokens3[0]);
    };

    "block comments are skipped"_test = [] {
        using namespace hycc;
        auto source_code1  = source_code{ std::u8string{ u8R"(/* line_one
*/line_two /* This is comment.
// So is this.
*/line_four)" } };
        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 3);

        // Newline at line 1 column 0.
        expect_token({ token_type::identifier, u8"line_two", 2, 3 }, tokens1[0]);
        expect_token({ token_type::whitespace, u8" ", 2, 11 }, tokens1[1]);
        expect_token({ token_type::identifier, u8"line_four", 4, 3 }, tokens1[2]);
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
        expect_token({ token_type::identifier, u8"foo", 2, 3 }, tokens2[0]);

        auto source_code3  = source_code{ std::u8string{ u8R"(/* first line
*/foo/* middle line
   last line */)" } };
        const auto tokens3 = tokenize(source_code3);
        expect(tokens3.size() == 1);
        expect_token({ token_type::identifier, u8"foo", 2, 3 }, tokens3[0]);
    };

    "block comment can be empty"_test = [] {
        using namespace hycc;
        auto source_code1  = source_code{ std::u8string{ u8R"(/**//**//**/)" } };
        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 0);

        auto source_code2  = source_code{ std::u8string{ u8R"(/**/foo/**/)" } };
        const auto tokens2 = tokenize(source_code2);
        expect(tokens2.size() == 1);
        expect_token({ token_type::identifier, u8"foo", 1, 5 }, tokens2[0]);
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
            expect_token({ token_type::error, std::u8string{ c8 }, 1, i + 1 }, tokens1[i]);
        }
    };

    "Tokens are maximally munched"_test = [] {
        using namespace hycc;

        auto source_code1  = source_code{ std::u8string{ u8"a+b" } };
        const auto tokens1 = tokenize(source_code1);

        expect(tokens1.size() == 3);
        const auto N1 = source_code1.sv().size();
        expect_token({ token_type::identifier, u8"a", 1, 1 }, tokens1[0]);
        expect_token({ token_type::operator_token, u8"+", 1, 2 }, tokens1[1]);
        expect_token({ token_type::identifier, u8"b", 1, N1 }, tokens1[2]);

        auto source_code2  = source_code{ std::u8string{ u8"a+++b" } };
        const auto tokens2 = tokenize(source_code2);

        expect(tokens2.size() == 4);
        const auto N2 = source_code2.sv().size();
        expect_token({ token_type::identifier, u8"a", 1, 1 }, tokens2[0]);
        expect_token({ token_type::operator_token, u8"++", 1, 2 }, tokens2[1]);
        expect_token({ token_type::operator_token, u8"+", 1, 4 }, tokens2[2]);
        expect_token({ token_type::identifier, u8"b", 1, N2 }, tokens2[3]);

        auto source_code3  = source_code{ std::u8string{ u8"a++-++b" } };
        const auto tokens3 = tokenize(source_code3);

        expect(tokens3.size() == 5);
        const auto N3 = source_code3.sv().size();
        expect_token({ token_type::identifier, u8"a", 1, 1 }, tokens3[0]);
        expect_token({ token_type::operator_token, u8"++", 1, 2 }, tokens3[1]);
        expect_token({ token_type::operator_token, u8"-", 1, 4 }, tokens3[2]);
        expect_token({ token_type::operator_token, u8"++", 1, 5 }, tokens3[3]);
        expect_token({ token_type::identifier, u8"b", 1, N3 }, tokens3[4]);

        auto source_code4  = source_code{ std::u8string{ u8"a+++++b" } };
        const auto tokens4 = tokenize(source_code4);

        expect(tokens4.size() == 5);
        const auto N4 = source_code4.sv().size();
        expect_token({ token_type::identifier, u8"a", 1, 1 }, tokens4[0]);
        expect_token({ token_type::operator_token, u8"++", 1, 2 }, tokens4[1]);
        expect_token({ token_type::operator_token, u8"++", 1, 4 }, tokens4[2]);
        expect_token({ token_type::operator_token, u8"+", 1, 6 }, tokens4[3]);
        expect_token({ token_type::identifier, u8"b", 1, N4 }, tokens4[4]);

        auto source_code5  = source_code{ std::u8string{ u8"a+++/* saving comment */++b" } };
        const auto tokens5 = tokenize(source_code5);

        expect(tokens5.size() == 5);
        const auto N5 = source_code5.sv().size();
        expect_token({ token_type::identifier, u8"a", 1, 1 }, tokens5[0]);
        expect_token({ token_type::operator_token, u8"++", 1, 2 }, tokens5[1]);
        expect_token({ token_type::operator_token, u8"+", 1, 4 }, tokens5[2]);
        expect_token({ token_type::operator_token, u8"++", 1, 25 }, tokens5[3]);
        expect_token({ token_type::identifier, u8"b", 1, N5 }, tokens5[4]);
    };

    "all tokens are regonized"_test = [] {
        using namespace hycc;

        auto source_code1 = source_code{ std::u8string{
            u8"<<=>>=<=>&=^=|=->++--||&&+=-=*=/=%=<<>><= >===!=!#%&+-/< = >?\\^|~$@" } };

        const auto tokens1 = tokenize(source_code1);
        expect(tokens1.size() == 42) << "got: " << tokens1.size();

        expect_token({ token_type::operator_token, u8"<<=", 1, 1 }, tokens1[0]);
        expect_token({ token_type::operator_token, u8">>=", 1, 4 }, tokens1[1]);
        expect_token({ token_type::operator_token, u8"<=>", 1, 7 }, tokens1[2]);
        expect_token({ token_type::operator_token, u8"&=", 1, 10 }, tokens1[3]);
        expect_token({ token_type::operator_token, u8"^=", 1, 12 }, tokens1[4]);
        expect_token({ token_type::operator_token, u8"|=", 1, 14 }, tokens1[5]);
        expect_token({ token_type::operator_token, u8"->", 1, 16 }, tokens1[6]);
        expect_token({ token_type::operator_token, u8"++", 1, 18 }, tokens1[7]);
        expect_token({ token_type::operator_token, u8"--", 1, 20 }, tokens1[8]);
        expect_token({ token_type::operator_token, u8"||", 1, 22 }, tokens1[9]);
        expect_token({ token_type::operator_token, u8"&&", 1, 24 }, tokens1[10]);
        expect_token({ token_type::operator_token, u8"+=", 1, 26 }, tokens1[11]);
        expect_token({ token_type::operator_token, u8"-=", 1, 28 }, tokens1[12]);
        expect_token({ token_type::operator_token, u8"*=", 1, 30 }, tokens1[13]);
        expect_token({ token_type::operator_token, u8"/=", 1, 32 }, tokens1[14]);
        expect_token({ token_type::operator_token, u8"%=", 1, 34 }, tokens1[15]);
        expect_token({ token_type::operator_token, u8"<<", 1, 36 }, tokens1[16]);
        expect_token({ token_type::operator_token, u8">>", 1, 38 }, tokens1[17]);
        expect_token({ token_type::operator_token, u8"<=", 1, 40 }, tokens1[18]);
        expect_token({ token_type::whitespace, u8" ", 1, 42 }, tokens1[19]);
        expect_token({ token_type::operator_token, u8">=", 1, 43 }, tokens1[20]);
        expect_token({ token_type::operator_token, u8"==", 1, 45 }, tokens1[21]);
        expect_token({ token_type::operator_token, u8"!=", 1, 47 }, tokens1[22]);
        expect_token({ token_type::operator_token, u8"!", 1, 49 }, tokens1[23]);
        expect_token({ token_type::operator_token, u8"#", 1, 50 }, tokens1[24]);
        expect_token({ token_type::operator_token, u8"%", 1, 51 }, tokens1[25]);
        expect_token({ token_type::operator_token, u8"&", 1, 52 }, tokens1[26]);
        expect_token({ token_type::operator_token, u8"+", 1, 53 }, tokens1[27]);
        expect_token({ token_type::operator_token, u8"-", 1, 54 }, tokens1[28]);
        expect_token({ token_type::operator_token, u8"/", 1, 55 }, tokens1[29]);
        expect_token({ token_type::operator_token, u8"<", 1, 56 }, tokens1[30]);
        expect_token({ token_type::whitespace, u8" ", 1, 57 }, tokens1[31]);
        expect_token({ token_type::operator_token, u8"=", 1, 58 }, tokens1[32]);
        expect_token({ token_type::whitespace, u8" ", 1, 59 }, tokens1[33]);
        expect_token({ token_type::operator_token, u8">", 1, 60 }, tokens1[34]);
        expect_token({ token_type::operator_token, u8"?", 1, 61 }, tokens1[35]);
        expect_token({ token_type::operator_token, u8"\\", 1, 62 }, tokens1[36]);
        expect_token({ token_type::operator_token, u8"^", 1, 63 }, tokens1[37]);
        expect_token({ token_type::operator_token, u8"|", 1, 64 }, tokens1[38]);
        expect_token({ token_type::operator_token, u8"~", 1, 65 }, tokens1[39]);
        expect_token({ token_type::operator_token, u8"$", 1, 66 }, tokens1[40]);
        expect_token({ token_type::operator_token, u8"@", 1, 67 }, tokens1[41]);
    };
}
