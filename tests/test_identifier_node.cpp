#include <boost/ut.hpp> // import boost.ut;

#include "hycc/ast.hpp"

int main() {
    using namespace boost::ut;
    using namespace hycc;

    "identifier_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::identifier_node{}; }));
    };

    "identifier_node can match unqualified-id"_test = [] {
        auto source       = source_code(u8"abc;");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto id           = ast::identifier_node{};

        expect(nothrow([&] { id.push(parser); }));
    };

    "identifier_node gives syntax error for empty list [part 1]"_test = [] {
        auto source       = source_code(u8";;");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto id           = ast::identifier_node{};

        expect(throws<hycc::syntax_error>([&] { id.push(parser); }));
    };

    "identifier_node gives syntax error for empty list [part 2]"_test = [] {
        auto source       = source_code(u8"***");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto id           = ast::identifier_node{};

        expect(throws<hycc::syntax_error>([&] { id.push(parser); }));
    };

    
    "identifier_node gives syntax error when :: is not followed with identifier token"_test = [] {
        auto source       = source_code(u8"a :: b :: c :: ");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto id           = ast::identifier_node{};

        expect(throws<hycc::syntax_error>([&] { id.push(parser); }));
    };

    "identifier_node gives syntax error when operator is not followed by operator token [part 1]"_test = [] {
        auto source       = source_code(u8"operator a");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto id           = ast::identifier_node{};

        expect(throws<hycc::syntax_error>([&] { id.push(parser); }));
    };

    "identifier_node gives syntax error when operator is not followed by operator token [part 2]"_test = [] {
        auto source       = source_code(u8"a::b::operator a");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto id           = ast::identifier_node{};

        expect(throws<hycc::syntax_error>([&] { id.push(parser); }));
    };

    "identifier_node can match qualified-id"_test = [] {
        auto source       = source_code(u8" :: b::c;");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto id           = ast::identifier_node{};

        expect(nothrow([&] { id.push(parser); }));
    };

    "identifier_node matches multiple id in row"_test = [] {
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

        expect(parser1.all_parsed());
        expect(parser2.all_parsed());
        expect(parser3.all_parsed());
        expect(id1 != id2);
        expect(id1 != id3);
        expect(id2 != id3);
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

    "identifier_node equality depends on potential operator part"_test = [] {
        auto source1       = source_code(u8"a::operator += ()");
        const auto tokens1 = tokenize(source1);
        auto parser1       = parser_t{ tokens1 };
        auto id1           = ast::identifier_node{};
        id1.push(parser1);

        auto source2       = source_code(u8"a::operator -= ()");
        const auto tokens2 = tokenize(source2);
        auto parser2       = parser_t{ tokens2 };
        auto id2           = ast::identifier_node{};
        id2.push(parser2);

        auto source3       = source_code(u8"a::operator += b::c");
        const auto tokens3 = tokenize(source3);
        auto parser3       = parser_t{ tokens3 };
        auto id3           = ast::identifier_node{};
        id3.push(parser3);

        expect(id1 == id1);
        expect(id2 == id2);
        expect(id3 == id3);
        expect(id1 != id2);
        expect(id1 == id3);
        expect(id2 != id3);
    };

    "identifier_node equality works when operator part matches"_test = [] {
        auto source1       = source_code(u8"a::operator<=> ()");
        const auto tokens1 = tokenize(source1);
        auto parser1       = parser_t{ tokens1 };
        auto id1           = ast::identifier_node{};
        id1.push(parser1);

        auto source2       = source_code(u8"b::operator<=>:  )");
        const auto tokens2 = tokenize(source2);
        auto parser2       = parser_t{ tokens2 };
        auto id2           = ast::identifier_node{};
        id2.push(parser2);

        auto source3       = source_code(u8"a::operator<=>: ");
        const auto tokens3 = tokenize(source3);
        auto parser3       = parser_t{ tokens3 };
        auto id3           = ast::identifier_node{};
        id3.push(parser3);

        expect(id1 == id1);
        expect(id2 == id2);
        expect(id3 == id3);
        expect(id1 != id2);
        expect(id1 == id3);
        expect(id2 != id3);
    };
}
