#include <boost/ut.hpp> // import boost.ut;

#include "hycc/ast.hpp"

int main() {
    using namespace boost::ut;
    using namespace hycc;

    "identifier_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::identifier_node{}; }));
    };

    "identifier_node can match identifier"_test = [] {
        auto source       = source_code(u8"abc;");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto id           = ast::identifier_node{};

        expect(nothrow([&] { id.push(parser); }));
    };

    "identifier_node can match ::"_test = [] {
        auto source       = source_code(u8"a::b::c;");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto id           = ast::identifier_node{};

        expect(nothrow([&] { id.push(parser); }));
    };

    "identifier_node does not ignore whitespace"_test = [] {
        auto source1       = source_code(u8"ab c");
        const auto tokens1 = tokenize(source1);
        auto parser1       = parser_t{ tokens1 };
        auto id1           = ast::identifier_node{};
        id1.push(parser1);

        auto source2       = source_code(u8"a bc");
        const auto tokens2 = tokenize(source2);
        auto parser2       = parser_t{ tokens2 };
        auto id2           = ast::identifier_node{};
        id2.push(parser2);

        auto source3       = source_code(u8"abc");
        const auto tokens3 = tokenize(source3);
        auto parser3       = parser_t{ tokens3 };
        auto id3           = ast::identifier_node{};
        id3.push(parser3);

        expect(not parser1.all_parsed());
        expect(not parser2.all_parsed());
        expect(parser3.all_parsed());
        expect(id1 != id2);
        expect(id1 != id3);
        expect(id2 != id3);
    };

    "identifier_node only matches :: not :"_test = [] {
        auto source1       = source_code(u8"a::b::c");
        const auto tokens1 = tokenize(source1);
        auto parser1       = parser_t{ tokens1 };
        auto id1           = ast::identifier_node{};
        id1.push(parser1);

        auto source2       = source_code(u8"a::b:: c");
        const auto tokens2 = tokenize(source2);
        auto parser2       = parser_t{ tokens2 };
        auto id2           = ast::identifier_node{};
        id2.push(parser2);

        auto source3       = source_code(u8"a::b:::c;");
        const auto tokens3 = tokenize(source3);
        auto parser3       = parser_t{ tokens3 };
        auto id3           = ast::identifier_node{};
        id3.push(parser3);

        expect(parser1.all_parsed());
        expect(not parser2.all_parsed());
        expect(not parser3.all_parsed());
        expect(id1 != id2);
        expect(id1 != id3);
        expect(id2 == id3);
    };

    "identifier_node is equality comparable"_test = [] {
        auto source1       = source_code(u8"abc");
        const auto tokens1 = tokenize(source1);
        auto parser1       = parser_t{ tokens1 };
        auto id1           = ast::identifier_node{};
        id1.push(parser1);

        auto source2       = source_code(u8"cba");
        const auto tokens2 = tokenize(source2);
        auto parser2       = parser_t{ tokens2 };
        auto id2           = ast::identifier_node{};
        id2.push(parser2);

        auto source3       = source_code(u8"abc::abc");
        const auto tokens3 = tokenize(source3);
        auto parser3       = parser_t{ tokens3 };
        auto id3           = ast::identifier_node{};
        id3.push(parser3);

        expect(id1 == id1);
        expect(id2 == id2);
        expect(id3 == id3);
        expect(id1 != id2);
        expect(id1 != id3);
        expect(id2 != id3);
    };
}
