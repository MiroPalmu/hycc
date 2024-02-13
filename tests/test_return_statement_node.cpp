#include <boost/ut.hpp> // import boost.ut;

#include "hycc/ast.hpp"

int main() {
    using namespace boost::ut;
    using namespace hycc;

    "return_statement_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::return_statement_node{}; }));
    };
}
