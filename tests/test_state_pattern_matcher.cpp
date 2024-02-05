
#include <boost/ut.hpp> // import boost.ut;

#include <string>

#include "hycc/sstd.hpp"
#include "hycc/state_pattern_matcher.hpp"

int main() {
    using namespace boost::ut;
    using namespace hycc;
    "sequence matcher can be constructed"_test = [] {
        using namespace state_pattern_matcher;
        struct state_type {
            std::string current                    = "";
            std::vector<std::string> begins        = {};
            std::vector<std::string> continuations = {};
            std::vector<std::string> ends          = {};
            constexpr void advance() { current = current + current; }
        };
        const auto foo = sstd::overloaded{
            [](predicate_tag, const auto& state) static -> bool { return state.current.empty(); },
            [](begin_tag, auto& state) static -> void {
                state.begins.push_back("foo");
                state.current = "foo";
            },
            [](continuation_tag, auto) static -> void {
                // This should never be called.
                expect(false) << "Logical error!";
            },
            [](end_tag, auto& state) static -> void { state.ends.push_back("foo"); }
        };

        const auto bar = sstd::overloaded{
            [](predicate_tag, const auto& state) static -> bool { return !state.current.empty(); },
            [](begin_tag, auto& state) static -> void { state.begins.push_back("bar"); },
            [](continuation_tag, auto& state) static -> void {
                state.continuations.push_back("bar");
                state.current = "bar";
            },
            [](end_tag, auto& state) static -> void { state.ends.push_back("bar"); }
        };

        auto matcher         = create_matcher_for<state_type>(foo, bar);
        auto until           = [](const auto& state) { return state.current == "barbar"; };
        const auto end_state = matcher(state_type{}, until);

        expect(end_state.current == "barbar");
        expect(end_state.begins == std::vector<std::string>{ "foo", "bar" });
        expect(end_state.continuations == std::vector<std::string>{ "bar" });
        expect(end_state.ends == std::vector<std::string>{ "foo", "bar" });
    };

    "state pattern matcher can only match one pattern at the same time"_test = [] {
        using namespace state_pattern_matcher;
        struct state_type {
            std::string current                    = "foo";
            std::vector<std::string> begins        = {};
            std::vector<std::string> continuations = {};
            std::vector<std::string> ends          = {};
            bool matching_foo                      = false;
            constexpr void advance() {}
        };
        const auto foo = sstd::overloaded{ [](predicate_tag, const auto& state) static -> bool {
                                              return state.current == "foo";
                                          },
                                           [](begin_tag, auto& state) static -> void {
                                               state.begins.push_back("foo");
                                               state.matching_foo = true;
                                           },
                                           [](continuation_tag, auto& state) static -> void {
                                               state.continuations.push_back("foo");
                                               state.current = "bar";
                                           },
                                           [](end_tag, auto& state) static -> void {
                                               state.ends.push_back("foo");
                                               state.matching_foo = false;
                                           } };

        const auto bar =
            sstd::overloaded{ [](predicate_tag, const auto& state) static -> bool {
                                 return (state.current == "foo" or state.current == "bar");
                             },
                              [](begin_tag, auto& state) static -> void {
                                  expect(not state.matching_foo);
                                  state.begins.push_back("bar");
                              },
                              [](continuation_tag, auto& state) static -> void {
                                  expect(not state.matching_foo);
                                  state.continuations.push_back("bar");
                                  state.current = "finish";
                              },
                              [](end_tag, auto& state) static -> void {
                                  state.ends.push_back("bar");
                              } };

        auto matcher         = create_matcher_for<state_type>(foo, bar);
        auto until           = [](const auto& state) { return state.current == "finish"; };
        const auto end_state = matcher(state_type{}, until);

        expect(end_state.current == "finish");
        expect(end_state.begins == std::vector<std::string>{ "foo", "bar" });
        expect(end_state.continuations == std::vector<std::string>{ "foo", "bar" });
        expect(end_state.ends == std::vector<std::string>{ "foo", "bar" });
    };
}
