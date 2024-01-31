#pragma once

namespace hycc {

enum class char_class_enum {
    whitespace,
    id,
    integer,
    literal_scope_operator,
    semantic_scope_operator,
    operator_unit,
    other
};

[[nodiscard]] constexpr auto classify_char(const char8_t c) -> char_class_enum {
    using enum char_class_enum;
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
        case (0x005B):
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

} // namespace hycc
