#include <boost/ut.hpp> // import boost.ut;

#include <string>

#include "sstd.hpp"

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

    "sequence matcher can be constructed"_test = [] {
        using namespace sstd::state_pattern_matcher;
        struct state_type {
            std::string current                    = "foo";
            std::vector<std::string> begins        = {};
            std::vector<std::string> continuations = {};
            std::vector<std::string> ends          = {};
            constexpr void advance() {}
        };
        const auto foo = sstd::overloaded{
            [](predicate_tag, const auto& state) static -> bool { return state.current == "foo"; },
            [](begin_tag, auto& state) static -> void { state.begins.push_back("foo"); },
            [](continuation_tag, auto& state) static -> void {
                state.continuations.push_back("foo");
                state.current = "bar";
            },
            [](end_tag, auto& state) static -> void { state.ends.push_back("foo"); }
        };

        const auto bar = sstd::overloaded{
            [](predicate_tag, const auto& state) static -> bool { return state.current == "bar"; },
            [](begin_tag, auto& state) static -> void { state.begins.push_back("bar"); },
            [](continuation_tag, auto& state) static -> void {
                state.continuations.push_back("bar");
                state.current = "second last to end";
            },
            [](end_tag, auto& state) static -> void {
                state.ends.push_back("bar");
                state.current = "finish";
            }
        };

        auto matcher         = create_matcher_for<state_type>(foo, bar);
        auto until           = [](const auto& state) { return state.current == "finish"; };
        const auto end_state = matcher(state_type{}, until);

        expect(end_state.current == "finish");
        expect(end_state.begins == std::vector<std::string>{ "foo", "bar" });
        expect(end_state.continuations == std::vector<std::string>{ "foo", "bar" });
        expect(end_state.ends == std::vector<std::string>{ "foo", "bar" });
    };
}
