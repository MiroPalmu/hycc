#include <boost/ut.hpp> // import boost.ut;

#include "hycc/ast.hpp"

int main() {
    using namespace boost::ut;
    using namespace hycc;

    "namespace_decleration_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::namespace_decleration_node{}; }));
    };
}
