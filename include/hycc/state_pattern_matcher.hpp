#pragma once

#include <concepts>
#include <optional>
#include <tuple>
#include <type_traits>
#include <variant>

namespace hycc {
namespace state_pattern_matcher {

struct predicate_tag {};
struct begin_tag {};
struct continuation_tag {};
struct end_tag {};

template<typename P, typename S>
concept pattern_for = requires(P p, S s) {
    { p(predicate_tag{}, s) } -> std::same_as<bool>;
    { p(begin_tag{}, s) } -> std::same_as<void>;
    { p(continuation_tag{}, s) } -> std::same_as<void>;
    { p(end_tag{}, s) } -> std::same_as<void>;
};

/// \p P... are types of function object which reprsesnt different patterns in the state.
///
/// State is of user spesified type \p S, which can be advanced `S::advance() -> void`.
///
/// Each pattern defines:
///
///     - predicate: `operator()(predicate_tag, S&) -> bool`
///     - begin: `operator()(begin_tag, S&) -> void`
///     - continuation: `operator()(continuation_tag, S&) -> void`
///     - end: `operator()(end_tag, S&) -> void `
///
/// matcher_t uses these in matcher_t::operator()(initial_state, until_predicate).
/// Predicate of patterns determine if the pattern matches that state.
///
template<typename S, pattern_for<S>... P>
    requires std::same_as<S, std::remove_cvref_t<S>>
class matcher_t {
    using patterns_t = std::tuple<std::remove_cvref_t<P>...>;
    patterns_t patterns_;

    using pattern_variant = std::variant<std::reference_wrapper<std::remove_cvref_t<P>>...>;

    /// Finds first pattern thats predicate is true. Throws if none found.
    constexpr auto find_pattern(const S& state) -> pattern_variant {
        auto found = std::optional<pattern_variant>{};

        auto helper = [&]<std::size_t I>() -> bool {
            if (std::get<I>(patterns_)(predicate_tag{}, state)) {
                found = std::ref(std::get<I>(patterns_));
                return false;
            }
            return true;
        };

        [&]<std::size_t... I>(std::index_sequence<I...>) {
            // Will short circuit, helper returns true until not.
            // This syntax is weird...
            ((helper.template operator()<I>()) and ...);
        }(std::make_index_sequence<sizeof...(P)>());

        // Throws if not found.
        return found.value();
    }

  public:
    [[nodiscard]] constexpr matcher_t(P... p) : patterns_{ p... } {}

    /// Modifies state until \p until_predicate is true.
    ///
    ///
    ///     1) If until_predicate(current_state) is true:
    ///         - Return current_state
    ///     2) Find first pattern p which predicate is true.
    ///         - Throws if none of the predicates is true.
    ///     3) Call begin of p.
    ///     4) Advance \p current_state.
    ///     5) If until_predicate(current_state) is true:
    ///         - Call end of p.
    ///         - Return current_state.
    ///     6) If predicate of p is true:
    ///         - Call continuation of p.
    ///         - Go to step 4.
    ///     7) else:
    ///         - Call end of p.
    ///         - Find new pattern p.
    ///         - Go to step 2.
    ///
    template<typename F>
    [[nodiscard]] constexpr auto operator()(S current_state, const F until_predicate) -> S {
        // Check if there is nothing to do.
        if (until_predicate(current_state)) return current_state;

        pattern_variant pattern_found = find_pattern(current_state);

        auto is_predicate_true = [&]() -> bool {
            return std::visit([&](auto pat) -> bool { return pat(predicate_tag{}, current_state); },
                              pattern_found);
        };

        auto tagged_call = [&]<typename Tag>(Tag) {
            std::visit([&](auto pat) -> void { pat(Tag{}, current_state); }, pattern_found);
        };

        goto jump_start;

    next_round:
        current_state.advance();
        if (until_predicate(current_state)) {
            tagged_call(end_tag{});
            return current_state;
        }

        if (is_predicate_true()) {
            // Continuing previous pattern.
            tagged_call(continuation_tag{});
        } else {
            // End of previous pattern.
            tagged_call(end_tag{});

            // Time to find new pattern.
            pattern_found = find_pattern(current_state);
        jump_start:
            tagged_call(begin_tag{});
        }
        goto next_round;
    }
};

template<typename S>
inline constexpr auto create_matcher_for = []<pattern_for<S>... P> [[nodiscard]] (P && ... p) {
    return matcher_t<S, P...>{ std::forward<P>(p)... };
};

} // namespace state_pattern_matcher
} // namespace hycc
