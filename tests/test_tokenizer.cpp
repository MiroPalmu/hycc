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
        using enum hycc::char_class_enum;
        struct char_class_pair {
            char8_t c;
            hycc::char_class_enum c_class;
        };
        const auto correct_chars_and_classes =
            std::vector<char_class_pair>{ { 0x0009, whitespace },
                                          { 0x000B, whitespace },
                                          { 0x000C, whitespace },
                                          { 0x0020, whitespace },
                                          { 0x000A, whitespace },
                                          { 0x0021, operator_unit },
                                          { 0x0022, literal_scope_operator },
                                          { 0x0023, operator_unit },
                                          { 0x0025, operator_unit },
                                          { 0x0026, operator_unit },
                                          { 0x0027, literal_scope_operator },
                                          { 0x0028, semantic_scope_operator },
                                          { 0x0029, semantic_scope_operator },
                                          { 0x002A, operator_unit },
                                          { 0x002B, operator_unit },
                                          { 0x002C, semantic_scope_operator },
                                          { 0x002D, operator_unit },
                                          { 0x002E, semantic_scope_operator },
                                          { 0x002F, operator_unit },
                                          { 0x0030, integer },
                                          { 0x0031, integer },
                                          { 0x0032, integer },
                                          { 0x0033, integer },
                                          { 0x0034, integer },
                                          { 0x0035, integer },
                                          { 0x0036, integer },
                                          { 0x0037, integer },
                                          { 0x0038, integer },
                                          { 0x0039, integer },
                                          { 0x003A, semantic_scope_operator },
                                          { 0x003B, semantic_scope_operator },
                                          { 0x003C, operator_unit },
                                          { 0x003D, operator_unit },
                                          { 0x003E, operator_unit },
                                          { 0x003F, operator_unit },
                                          { 0x0041, id },
                                          { 0x0042, id },
                                          { 0x0043, id },
                                          { 0x0044, id },
                                          { 0x0045, id },
                                          { 0x0046, id },
                                          { 0x0047, id },
                                          { 0x0048, id },
                                          { 0x0049, id },
                                          { 0x004A, id },
                                          { 0x004B, id },
                                          { 0x004C, id },
                                          { 0x004D, id },
                                          { 0x004E, id },
                                          { 0x004F, id },
                                          { 0x0050, id },
                                          { 0x0051, id },
                                          { 0x0052, id },
                                          { 0x0054, id },
                                          { 0x0054, id },
                                          { 0x0055, id },
                                          { 0x0056, id },
                                          { 0x0057, id },
                                          { 0x0058, id },
                                          { 0x0059, id },
                                          { 0x005A, id },
                                          { 0x005B, semantic_scope_operator },
                                          { 0x005C, operator_unit },
                                          { 0x005D, semantic_scope_operator },
                                          { 0x005E, operator_unit },
                                          { 0x005F, id },
                                          { 0x0061, id },
                                          { 0x0062, id },
                                          { 0x0063, id },
                                          { 0x0064, id },
                                          { 0x0065, id },
                                          { 0x0066, id },
                                          { 0x0067, id },
                                          { 0x0068, id },
                                          { 0x0069, id },
                                          { 0x006A, id },
                                          { 0x006B, id },
                                          { 0x006C, id },
                                          { 0x006D, id },
                                          { 0x006E, id },
                                          { 0x006F, id },
                                          { 0x0070, id },
                                          { 0x0071, id },
                                          { 0x0072, id },
                                          { 0x0073, id },
                                          { 0x0074, id },
                                          { 0x0075, id },
                                          { 0x0076, id },
                                          { 0x0077, id },
                                          { 0x0078, id },
                                          { 0x0079, id },
                                          { 0x007A, id },
                                          { 0x007B, semantic_scope_operator },
                                          { 0x007C, operator_unit },
                                          { 0x007D, semantic_scope_operator },
                                          { 0x007E, operator_unit },
                                          { 0x0024, operator_unit },
                                          { 0x0040, operator_unit },
                                          { 0x0060, literal_scope_operator } };

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
            using enum hycc::char_class_enum;
            expect(hycc::classify_char(c) == other) << std::format("{:X} is classified wrong!", c);
        });
    };
}
