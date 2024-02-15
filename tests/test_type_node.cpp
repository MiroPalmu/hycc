#include <boost/ut.hpp> // import boost.ut;

#include "hycc/ast.hpp"

int main() {
    using namespace boost::ut;
    using namespace hycc;

    "type_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::type_node{}; }));
    };

    "type_node can match basic type"_test = [] {
        auto source       = source_code(u8"int;");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto type         = ast::type_node{};
        type.push(parser);

        expect(not type.is_const());
        expect(not type.is_pointer());
        expect(type.is_regular_type());
    };

    "type_node can detect syntax error"_test = [] {
        auto source       = source_code(u8"**;");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto type         = ast::type_node{};

        expect(throws<hycc::syntax_error>([&] { type.push(parser); }));
    };

    "type_node can detect constness"_test = [] {
        auto source       = source_code(u8"const int;");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto type         = ast::type_node{};
        type.push(parser);

        expect(type.is_const());
        expect(not type.is_pointer());
        expect(type.is_regular_type());
    };

    "type_node can detect pointer types"_test = [] {
        auto source       = source_code(u8"* int;");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto type         = ast::type_node{};
        type.push(parser);

        expect(not type.is_const());
        expect(not type.is_regular_type());
        expect(type.is_pointer());
        expect(type.pointed_type().is_regular_type());
    };

    "type_node can detect const pointer types"_test = [] {
        auto source       = source_code(u8"const*int;");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto type         = ast::type_node{};
        type.push(parser);

        expect(type.is_const());
        expect(not type.is_regular_type());
        expect(type.is_pointer());
        expect(type.pointed_type().is_regular_type());
    };

    "type_node can detect pointer to pointer types"_test = [] {
        auto source       = source_code(u8"**int;");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto type         = ast::type_node{};
        type.push(parser);

        expect(not type.is_const());
        expect(not type.is_regular_type());
        expect(type.is_pointer());

        expect(not type.pointed_type().is_regular_type());
        expect(type.pointed_type().is_pointer());
        expect(not type.pointed_type().is_const());

        expect(type.pointed_type().pointed_type().is_regular_type());
        expect(not type.pointed_type().pointed_type().is_pointer());
        expect(not type.pointed_type().pointed_type().is_const());
    };

    "type_node can detect pointer to const pointer types"_test = [] {
        auto source       = source_code(u8"* const * const int;");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto type         = ast::type_node{};
        type.push(parser);

        expect(not type.is_const());
        expect(not type.is_regular_type());
        expect(type.is_pointer());

        expect(not type.pointed_type().is_regular_type());
        expect(type.pointed_type().is_pointer());
        expect(type.pointed_type().is_const());

        expect(type.pointed_type().pointed_type().is_regular_type());
        expect(not type.pointed_type().pointed_type().is_pointer());
        expect(type.pointed_type().pointed_type().is_const());
    };
}
