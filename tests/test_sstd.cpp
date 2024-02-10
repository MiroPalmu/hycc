#include <boost/ut.hpp> // import boost.ut;

#include <string>

#include "hycc/sstd.hpp"

int main() {
    using namespace boost::ut;
    using namespace hycc;

    "ownership can be created"_test = [] {
        struct resource {
            int a;
            float b;
            constexpr bool operator==(const resource&) const = default;
        };

        const auto ownership1 = sstd::make_shared_object<resource>(3, 2.3f);
        expect(ownership1.value().a == 3);
        expect(ownership1.value().b == 2.3f);
    };

    "ownership can be created 1"_test = [] {
        struct resource {
            int a;
            float b;
            constexpr bool operator==(const resource&) const = default;
        };

        const auto ownership1 = sstd::make_shared_object<resource>(3, 2.3f);
        expect(ownership1.value().a == 3);
        expect(ownership1.value().b == 2.3f);

        const auto ownership2 = sstd::make_shared_object<resource>(3, 2.3f);
        expect(ownership2.value().a == 3);
        expect(ownership2.value().b == 2.3f);
    };

    "ownership can be moved"_test = [] {
        struct resource {
            int a;
            float b;
            constexpr bool operator==(const resource&) const = default;
        };

        const auto ownership1 = sstd::make_shared_object<resource>(3, 2.3f);
        auto ownership2       = ownership1;
        auto ownership3       = std::move(ownership2);
        expect(ownership3.value().a == 3);
        expect(ownership3.value().b == 2.3f);
    };

    "ownership can be destructed"_test = [] {
        struct resource {
            int a;
            float b;
            constexpr bool operator==(const resource&) const = default;
        };

        const auto ownership1 = sstd::make_shared_object<resource>(3, 2.3f);
        { auto ownership2 = ownership1; }
        expect(ownership1.value().a == 3);
        expect(ownership1.value().b == 2.3f);
    };

    "limited truth can be created"_test = [] {
        auto limited_truth = sstd::limited_truth{ 42 };
        expect(limited_truth.get_truth());
    };

    "limited truth can be used"_test = [] {
        auto limited_truth = sstd::limited_truth{ 4 };

        expect(limited_truth.get_truth());
        expect(limited_truth.get_truth());
        expect(limited_truth.get_truth());
        expect(limited_truth.get_truth());
        expect(not limited_truth.get_truth());
        expect(not limited_truth.get_truth());
    };

    "limited truth can be set"_test = [] {
        auto limited_truth = sstd::limited_truth{ 4 };

        expect(limited_truth.get_truth());
        expect(limited_truth.get_truth());
        expect(limited_truth.get_truth());
        expect(limited_truth.get_truth());
        expect(not limited_truth.get_truth());
        expect(not limited_truth.get_truth());
        
        limited_truth.set_truth_amount(2);
        expect(limited_truth.get_truth());
        expect(limited_truth.get_truth());
        expect(not limited_truth.get_truth());
        expect(not limited_truth.get_truth());

        limited_truth.set_truth_amount(2);
        limited_truth.set_truth_amount(2);
        expect(limited_truth.get_truth());
        limited_truth.set_truth_amount(1);
        expect(limited_truth.get_truth());
        expect(not limited_truth.get_truth());
        expect(not limited_truth.get_truth());

        limited_truth.set_truth_amount(2);
        limited_truth.set_truth_amount(2);
        expect(limited_truth.get_truth());
        limited_truth.set_truth_amount(2);
        expect(limited_truth.get_truth());
        expect(limited_truth.get_truth());
        expect(not limited_truth.get_truth());
        limited_truth.set_truth_amount(2);
        expect(limited_truth.get_truth());
        expect(limited_truth.get_truth());
        expect(not limited_truth.get_truth());
        expect(not limited_truth.get_truth());
    };
}
