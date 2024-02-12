
#include <boost/ut.hpp> // import boost.ut;

//#include <format>
//#include <ranges>
//#include <source_location>
//#include <variant>
#include <vector>

#include "hycc/ast.hpp"
#include "hycc/tokenizer.hpp"

/* Tests exits:
 *
 * [x] Identifier node
 * [x] Function argument node
 * [x] Type node
 * [x] Expression node
 * [x] Class decleration node
 * [x] Function decleration node
 * [x] Namespace decleration node
 * [x] Data decleration node
 * [x] Decleration parsing node
 * [x] Scope node
 * [x] If statement
 * [x] For loop statement
 * [x] Break statement
 * [x] Continue statement
 * [x] Return statement
 *
 * [X] all abstract nodes
 **/

// constexpr void
//     expect_ordered_property(const std::vector<hycc::ast::ordered_property>& expected,
//                             const std::span<hycc::ast::ordered_property const> got,
//                             const std::source_location loc = std::source_location::current()) {
//     const auto loc_info = std::format("[actually at: {}]\n\n\t", loc.line());
//     boost::ut::expect(expected.size() == got.size())
//         << loc_info
//         << std::format("Expected {} ordered property, got {}", expected.size(), got.size());
//     const auto get_name =
//         hycc::sstd::overloaded{
//             [](hycc::ast::scope_node) { return "scope node"; },
//             [](hycc::ast::continue_statement_node) { return "continue statement node"; }
//         };
//     auto error_msg = [&](const hycc::ast::ordered_property& e,
//                          const hycc::ast::ordered_property& g) {
//         return std::format("{}\n\texpected ordered property `{}`, got `{}`",
//                            loc_info,
//                            std::visit(get_name, e),
//                            std::visit(get_name, g));
//     };
//
//     for (const auto& [e, g] : std::views::zip(expected, got)) {
//         boost::ut::expect(e.index() == g.index()) << error_msg(e, g);
//     }
// };

int main() {
    using namespace boost::ut;
    using namespace hycc;
    "identifier_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::identifier_node{}; }));
    };

    "function_argument_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::function_argument_node{}; }));
    };

    "type_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::type_node{}; }));
    };

    "expression_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::expression_node{}; }));
    };

    "class_decleration_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::class_decleration_node{}; }));
    };

    "function_decleration_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::function_decleration_node{}; }));
    };

    "namespace_decleration_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::namespace_decleration_node{}; }));
    };

    "data_decleration_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::data_decleration_node{}; }));
    };

    "decleration_parsing_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::decleration_parsing_node{}; }));
    };

    "scope_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::scope_node{}; }));
    };

    "if_statement_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::if_statement_node{}; }));
    };

    "for_loop_statement_node_ can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::for_loop_statement_node_{}; }));
    };

    "break_statement_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::break_statement_node{}; }));
    };

    "continue_statement_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::continue_statement_node{}; }));
    };

    "return_statement_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::return_statement_node{}; }));
    };

    "nested_scope can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::nested_scope{}; }));
    };
    "statement_scope can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::statement_scope{}; }));
    };
    "namespace_scope can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::namespace_scope{}; }));
    };
    "function_scope can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::function_scope{}; }));
    };
    "class_scope can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::class_scope{}; }));
    };
    "ordered_property   can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::ordered_property{}; }));
    };
    "unordered_property can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::unordered_property{}; }));
    };
}
