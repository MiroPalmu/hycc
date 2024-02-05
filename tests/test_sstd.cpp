#include <boost/ut.hpp> // import boost.ut;

#include <string>

#include "hycc/sstd.hpp"

int main() {
    using namespace boost::ut;
    using namespace hycc;

    "ownership can be shared"_test = [] {
        struct resource {
            int a;
            float b;
            constexpr bool operator==(const resource&) const = default;
        };

        const auto ownership1 = sstd::make_shared_object<resource>(3, 2.3f);
        auto ownership2       = ownership1;
        auto ownership3       = std::move(ownership2);
        expect(ownership3.value() == ownership1.value());
        ownership1.value().a = 4;
        ownership1.value().b = 4.5f;
        expect(ownership3.value().a == 4);
        expect(ownership3.value().b == 4.5f);
    };
}
