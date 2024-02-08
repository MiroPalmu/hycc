#include <algorithm>
#include <boost/ut.hpp> // import boost.ut;
#include <format>
#include <string>
#include <tuple>
#include <vector>

#include "hycc/tokenizer.hpp"

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
}
