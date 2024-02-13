#include <boost/ut.hpp> // import boost.ut;

#include "hycc/ast.hpp"

int main() {
    using namespace boost::ut;
    using namespace hycc;

    "function_decleration_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::function_decleration_node{}; }));
    };
}
