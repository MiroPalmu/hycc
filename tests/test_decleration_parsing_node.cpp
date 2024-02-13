#include <boost/ut.hpp> // import boost.ut;

#include "hycc/ast.hpp"

int main() {
    using namespace boost::ut;
    using namespace hycc;

    "decleration_parsing_node can be constructed"_test = [] {
        expect(nothrow([] { [[maybe_unused]] auto _ = ast::decleration_parsing_node{}; }));
    };
}
