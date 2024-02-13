
#include <boost/ut.hpp> // import boost.ut;

#include <format>
#include <ranges>
#include <source_location>
#include <variant>
#include <vector>

#include "hycc/ast.hpp"
#include "hycc/parser.hpp"
#include "hycc/tokenizer.hpp"


constexpr void
    expect_ordered_property(const std::vector<hycc::ast::ordered_property>& expected,
                            const std::span<hycc::ast::ordered_property const> got,
                            const std::source_location loc = std::source_location::current()) {
    const auto loc_info = std::format("[actually at: {}]\n\n\t", loc.line());
    boost::ut::expect(expected.size() == got.size())
        << loc_info
        << std::format("Expected {} ordered property, got {}", expected.size(), got.size());
    const auto get_name = hycc::sstd::overloaded{
        [](hycc::ast::nested_scope) { return " nested_scope"; },
        [](hycc::ast::if_statement_node) { return " if_statement_node"; },
        [](hycc::ast::for_loop_statement_node) { return " for_loop_statement_node"; },
        [](hycc::ast::break_statement_node) { return " break_statement_node"; },
        [](hycc::ast::continue_statement_node) { return " continue_statement_node"; },
        [](hycc::ast::return_statement_node) { return " return_statement_node"; },
        [](hycc::ast::namespace_decleration_node) { return " namespace_decleration_node"; },
        [](hycc::ast::expression_node) { return " expression_node"; },
        [](hycc::ast::data_decleration_node) { return " data_decleration_node"; }

    };
    auto error_msg = [&](const hycc::ast::ordered_property& e,
                         const hycc::ast::ordered_property& g) {
        return std::format("{}\n\texpected ordered property `{}`, got `{}`",
                           loc_info,
                           std::visit(get_name, e),
                           std::visit(get_name, g));
    };

    for (const auto& [e, g] : std::views::zip(expected, got)) {
        boost::ut::expect(e.index() == g.index()) << error_msg(e, g);
    }
};

int main() {
    using namespace boost::ut;
    using namespace hycc;

    "scope_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::scope_node{}; }));
    };

    "global scope_node detects if it ends like non-global scope"_test = [] {
        auto source       = source_code(u8"}");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };

        auto global_scope = ast::scope_node{};
        global_scope.mark_as_global_scope();

        expect(throws<syntax_error>([&] { global_scope.push(parser); }));
    };

    "global scope_node detects if end of file"_test = [] {
        auto source       = source_code(u8" ");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };

        auto global_scope = ast::scope_node{};
        global_scope.mark_as_global_scope();

        expect(nothrow([&] { global_scope.push(parser); }));
    };

    "non-global scope_node detects if it ends like global scope"_test = [] {
        auto source       = source_code(u8" ");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };

        auto non_global_scope = ast::scope_node{};
        expect(throws<syntax_error>([&] { non_global_scope.push(parser); }));
    };

    "non-global scope_node matches end of scope"_test = [] {
        auto source       = source_code(u8" } ");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };

        auto non_global_scope = ast::scope_node{};
        expect(nothrow([&] { non_global_scope.push(parser); }));
    };

    "scope_node matches nested scope"_test = [] {
        auto source       = source_code(u8"{} {}");
        const auto tokens = tokenize(source);
        auto parser       = parser_t{ tokens };
        auto global_scope = ast::scope_node{};
        global_scope.mark_as_global_scope();
        global_scope.push(parser);
        expect_ordered_property({ ast::nested_scope{}, ast::nested_scope{} },
                                global_scope.get_ordered_property());
    };

}
