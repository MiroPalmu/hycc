#include <boost/ut.hpp> // import boost.ut;

#include "hycc/ast.hpp"

int main() {
    using namespace boost::ut;
    using namespace hycc;

    "if_statement_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::if_statement_node{}; }));
    };
}
