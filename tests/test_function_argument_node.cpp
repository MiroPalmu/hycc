#include <boost/ut.hpp> // import boost.ut;

#include "hycc/ast.hpp"
#include "hycc/parser.hpp"
#include "hycc/tokenizer.hpp"

int main() {
    using namespace boost::ut;
    using namespace hycc;

    "function_argument_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::function_argument_node{}; }));
    };

    "function_arguments matches argument types"_test = [] {
        auto source       = source_code(u8" arg1: const int, arg2: *double);");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto args         = ast::function_argument_node{};

        args.push(parser);
        const auto a = args.get_args();
        expect(a.size() == 2);

        expect(a.front().type.has_value());
        expect(a.front().type.value().is_regular_type());
        expect(a.front().type.value().is_const());

        expect(a.back().type.has_value());
        expect(a.back().type.value().is_pointer());
        expect(not a.back().type.value().is_const());
    };

    "function_arguments defaults to in passing"_test = [] {
        auto source       = source_code(u8" arg1: const int, arg2: *double);");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto args         = ast::function_argument_node{};
        args.push(parser);

        const auto a = args.get_args();
        expect(a.size() == 2);

        expect(a.front().type.has_value());
        expect(a.front().pass == ast::passing_type::in);

        expect(a.back().type.has_value());
        expect(a.back().pass == ast::passing_type::in);
    };

    "function_arguments matches in"_test = [] {
        auto source       = source_code(u8"in arg1: const int, in arg2: *double);");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto args         = ast::function_argument_node{};
        args.push(parser);

        const auto a = args.get_args();
        expect(a.size() == 2);

        expect(a.front().type.has_value());
        expect(a.front().pass == ast::passing_type::in);

        expect(a.back().type.has_value());
        expect(a.back().pass == ast::passing_type::in);
    };

    "function_arguments matches inout"_test = [] {
        auto source       = source_code(u8"inout arg1: const int, inout arg2: *double);");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto args         = ast::function_argument_node{};
        args.push(parser);

        const auto a = args.get_args();
        expect(a.size() == 2);

        expect(a.front().type.has_value());
        expect(a.front().pass == ast::passing_type::inout);

        expect(a.back().type.has_value());
        expect(a.back().pass == ast::passing_type::inout);
    };

    "function_arguments matches out"_test = [] {
        auto source       = source_code(u8"out arg1: const int, out arg2: *double);");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto args         = ast::function_argument_node{};
        args.push(parser);

        const auto a = args.get_args();
        expect(a.size() == 2);

        expect(a.front().type.has_value());
        expect(a.front().pass == ast::passing_type::out);

        expect(a.back().type.has_value());
        expect(a.back().pass == ast::passing_type::out);
    };

    "function_arguments matches move"_test = [] {
        auto source       = source_code(u8"out arg1: const int, out arg2: *double);");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto args         = ast::function_argument_node{};
        args.push(parser);

        const auto a = args.get_args();
        expect(a.size() == 2);

        expect(a.front().type.has_value());
        expect(a.front().pass == ast::passing_type::out);

        expect(a.back().type.has_value());
        expect(a.back().pass == ast::passing_type::out);
    };

    "function_arguments matches move"_test = [] {
        auto source       = source_code(u8"move arg1: const int, move arg2: *double);");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto args         = ast::function_argument_node{};
        args.push(parser);

        const auto a = args.get_args();
        expect(a.size() == 2);

        expect(a.front().type.has_value());
        expect(a.front().pass == ast::passing_type::move);

        expect(a.back().type.has_value());
        expect(a.back().pass == ast::passing_type::move);
    };

    "function_arguments matches copy"_test = [] {
        auto source       = source_code(u8"copy arg1: const int, copy arg2: *double);");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto args         = ast::function_argument_node{};
        args.push(parser);

        const auto a = args.get_args();
        expect(a.size() == 2);

        expect(a.front().type.has_value());
        expect(a.front().pass == ast::passing_type::copy);

        expect(a.back().type.has_value());
        expect(a.back().pass == ast::passing_type::copy);
    };

    "function_arguments matches foward"_test = [] {
        auto source       = source_code(u8"forward arg1: const int, forward arg2: *double);");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto args         = ast::function_argument_node{};
        args.push(parser);

        const auto a = args.get_args();
        expect(a.size() == 2);

        expect(a.front().type.has_value());
        expect(a.front().pass == ast::passing_type::forward);

        expect(a.back().type.has_value());
        expect(a.back().pass == ast::passing_type::forward);
    };

    "function_arguments can be typeless"_test = [] {
        auto source       = source_code(u8"arg1, forward arg2);");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto args         = ast::function_argument_node{};
        args.push(parser);

        const auto a = args.get_args();
        expect(a.size() == 2);

        expect(not a.front().type.has_value());

        expect(not a.back().type.has_value());
    };

    "function_arguments can be empty"_test = [] {
        auto source       = source_code(u8");");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto args         = ast::function_argument_node{};
        args.push(parser);

        const auto a = args.get_args();
        expect(a.size() == 0);
    };

}
