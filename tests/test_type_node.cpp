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
        expect(not type.is_function());
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
        expect(not type.is_function());
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
        expect(not type.is_function());
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
        expect(not type.is_function());
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
        expect(not type.is_function());

        expect(not type.pointed_type().is_regular_type());
        expect(type.pointed_type().is_pointer());
        expect(not type.pointed_type().is_const());
        expect(not type.pointed_type().is_function());

        expect(type.pointed_type().pointed_type().is_regular_type());
        expect(not type.pointed_type().pointed_type().is_pointer());
        expect(not type.pointed_type().pointed_type().is_const());
        expect(not type.pointed_type().is_function());
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
        expect(not type.is_function());

        expect(not type.pointed_type().is_regular_type());
        expect(type.pointed_type().is_pointer());
        expect(type.pointed_type().is_const());
        expect(not type.pointed_type().is_function());

        expect(type.pointed_type().pointed_type().is_regular_type());
        expect(not type.pointed_type().pointed_type().is_pointer());
        expect(type.pointed_type().pointed_type().is_const());
        expect(not type.pointed_type().pointed_type().is_function());
    };

    "type_node can detect function with no arguments"_test = [] {
        auto source       = source_code(u8"() -> *int = ");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto type         = ast::type_node{};
        type.push(parser);

        expect(type.is_function());
        expect(not type.is_const());
        expect(not type.is_regular_type());
        expect(not type.is_pointer());

        expect(type.function().args.get_args().empty());
        expect(not type.function().return_type.is_regular_type());
        expect(not type.function().return_type.is_const());
        expect(type.function().return_type.is_pointer());
        expect(not type.function().return_type.is_function());
    };

    "type_node can detect function with arguments"_test = [] {
        auto source       = source_code(u8"(foo: int, move bar: float) -> *int = ");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto type         = ast::type_node{};
        type.push(parser);

        expect(type.is_function());
        expect(not type.is_const());
        expect(not type.is_regular_type());
        expect(not type.is_pointer());

        expect(type.function().args.get_args().size() == 2);

        expect(not type.function().return_type.is_regular_type());
        expect(not type.function().return_type.is_const());
        expect(type.function().return_type.is_pointer());
        expect(not type.function().return_type.is_function());
    };

    "type_node can detect function which return function"_test = [] {
        auto source       = source_code(u8"(foo) -> (bar) -> int = ");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto type         = ast::type_node{};
        type.push(parser);

        expect(type.is_function());
        expect(not type.is_const());
        expect(not type.is_regular_type());
        expect(not type.is_pointer());

        expect(type.function().args.get_args().size() == 1);

        expect(not type.function().return_type.is_regular_type());
        expect(not type.function().return_type.is_const());
        expect(not type.function().return_type.is_pointer());
        expect(type.function().return_type.is_function());

        expect(type.function().return_type.function().args.get_args().size() == 1);

        expect(type.function().return_type.function().return_type.is_regular_type());
        expect(not type.function().return_type.function().return_type.is_const());
        expect(not type.function().return_type.function().return_type.is_pointer());
        expect(not type.function().return_type.function().return_type.is_function());
    };

    "type_node can detect syntax error when no return type separator"_test = [] {
        auto source       = source_code(u8"(foo: int, move bar: float) type = ");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto type         = ast::type_node{};

        expect(throws<syntax_error>([&] { type.push(parser); }));
    };

    "type_node can detect syntax error when return type separator contains whitespace"_test = [] {
        auto source       = source_code(u8"(foo: int, move bar: float) - >  = ");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto type         = ast::type_node{};

        expect(throws<syntax_error>([&] { type.push(parser); }));
    };

    "type_node can detect syntax error when no return type"_test = [] {
        auto source       = source_code(u8"(foo: int, move bar: float) ->  = ");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto type         = ast::type_node{};

        expect(throws<syntax_error>([&] { type.push(parser); }));
    };
}
