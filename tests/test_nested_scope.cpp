
#include <boost/ut.hpp> // import boost.ut;

#include "hycc/ast.hpp"

int main() {
    using namespace boost::ut;
    using namespace hycc;

    "nested_scope can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::nested_scope{}; }));
    };
}
