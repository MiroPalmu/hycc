#pragma once

#include <concepts>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

/// @file supplement std (std)

namespace hycc {
namespace sstd {

template<typename>
class ownership;

template<typename T>
class control_block {
    std::size_t count_;
    T shared_;

    friend ownership<T>;

  public:
    [[nodiscard]] constexpr control_block(T&& args)
        : count_{ 0 },
          shared_{ std::forward<T>(args) } {}

    constexpr control_block(const control_block&)           = delete;
    constexpr control_block(control_block&&)                = delete;
    constexpr control_block operator=(const control_block&) = delete;
    constexpr control_block operator=(control_block&&)      = delete;
    constexpr ~control_block()                              = default;

    [[nodiscard]] constexpr auto get_ownership() -> ownership<T> {
        ++count_;
        return { this };
    }

    [[nodiscard]] constexpr auto value(this auto&& me) -> T& { return me.shared_; }
};

/// Makes not not thread safe constexpr shared_ptr clone.
template<typename T, typename... Args>
constexpr auto make_shared_object(Args&&... args) -> ownership<T> {
    return (new control_block<T>{ T{ std::forward<Args>(args)... } })->get_ownership();
}

/// Reference counted (using control_block) ownership of stack allocation of object
template<typename T>
class ownership {
    friend class control_block<T>;

    mutable control_block<T>* block_;
    constexpr ownership(control_block<T>* const block) : block_{ block } {}

  public:
    constexpr ownership() = delete;
    constexpr ownership(const ownership& that) { *this = that.block_->get_ownership(); }
    constexpr ownership& operator=(const ownership& that) {
        return *this = that.block_->get_ownership();
    }
    /// No reason to move one pointer.
    constexpr ownership(ownership&&) = default;
    /// No reason to move one pointer.
    constexpr ownership& operator=(ownership&&) = default;

    constexpr ~ownership() {
        // If no other ownership exits, clean up.
        if (--(block_->count_) == 0) delete block_;
    }

    [[nodiscard]] constexpr auto value(this auto&& me) -> T& { return me.block_->value(); }
};

/// Helper for creating function objects.
template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
/// Explicit deduction guide.
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

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
/// State is of user spesified type \p S, which has member function `S::advance() -> void`.
///
/// Each pattern defines:
///
///     - predicate: `operator()(predicate_tag, const S&) -> bool`
///     - begin: `operator()(begin_tag, S&) -> void`
///     - continuation: `operator()(continuation_tag, S&) -> void`
///     - end: `operator()(end_tag, S&) -> void `
///
template<typename S, pattern_for<S>... P>
    requires std::same_as<S, std::remove_cvref_t<S>>
class matcher_t {
    std::tuple<P...> patterns_;

    template<std::size_t, typename...>
    struct mask_type_impl_;

    template<std::size_t amount, typename... Tail>
        requires(amount == 0)
    struct mask_type_impl_<amount, Tail...> {
        using type = std::tuple<Tail...>;
    };

    template<std::size_t amount, typename... Tail>
        requires(amount > 0)
    struct mask_type_impl_<amount, Tail...> {
        using type = mask_type_impl_<amount - 1, bool, Tail...>::type;
    };

    using mask_type = mask_type_impl_<sizeof...(P)>::type;

    constexpr auto create_predicate_mask(S& state) -> mask_type {
        return [&]<std::size_t... I>(std::index_sequence<I...>) {
            return std::make_tuple(std::get<I>(patterns_)(predicate_tag{}, state)...);
        }(std::make_index_sequence<sizeof...(P)>());
    }

    static constexpr auto invert_mask(const mask_type& mask) -> mask_type {
        return [&]<std::size_t... I>(std::index_sequence<I...>) {
            return std::make_tuple(not std::get<I>(mask)...);
        }(std::make_index_sequence<sizeof...(P)>());
    }

    static constexpr auto and_mask(const mask_type& lhs, const mask_type& rhs) -> mask_type {
        return [&]<std::size_t... I>(std::index_sequence<I...>) {
            return std::make_tuple((std::get<I>(lhs) and std::get<I>(rhs))...);
        }(std::make_index_sequence<sizeof...(P)>());
    }

    template<typename Tag>
    constexpr auto tag_call_with_mask(const mask_type& mask, S& state) -> void {
        auto filtered_call = [&]<std::size_t I>() -> void {
            if (std::get<I>(mask)) std::get<I>(patterns_)(Tag{}, state);
        };

        [&]<std::size_t... I>(std::index_sequence<I...>) {
            // This template is weird :)
            (filtered_call.template operator()<I>(), ...);
        }(std::make_index_sequence<sizeof...(P)>());
    }

  public:
    [[nodiscard]] constexpr matcher_t(P... p) : patterns_{ p... } {}

    /// Modifies state until `F::operator()(cons S&) -> bool` is true.
    ///
    /// Algorithm (every call happens in order of \p P):
    ///
    ///     1) Call predicate of each pattern and store result (predicate mask, pm for short).
    ///     2) Call begin of patterns filtered with pm.
    ///     3) Store pm as "previous predicate mask", ppm for short.
    ///     4) Call advance on the state.
    ///     5) Create new predicate mask.
    ///     6) Call begin of patterns filtered with mask (inverse ppm AND pm).
    ///     7) Call continuation for filtered with mask (ppm AND pm).
    ///     8) Call ends for all patterns filtered with mask (ppm AND inverse pm).
    ///     9) If `until_predicate(current_state)` is ture, go to step 11)
    ///     10) Go to step 3.
    ///
    template<typename F>
    [[nodiscard]] constexpr auto operator()(S current_state, const F until_predicate) -> S {
        auto pm = create_predicate_mask(current_state);
        tag_call_with_mask<begin_tag>(pm, current_state);
        auto ppm = pm;
        while (true) {
            ppm = pm;
            current_state.advance();
            pm = create_predicate_mask(current_state);
            tag_call_with_mask<begin_tag>(and_mask(invert_mask(ppm), pm), current_state);
            tag_call_with_mask<continuation_tag>(and_mask(ppm, pm), current_state);
            tag_call_with_mask<end_tag>(and_mask(ppm, invert_mask(pm)), current_state);

            if (until_predicate(current_state)) return current_state;
        }
    }
};

template<typename S>
constexpr auto create_matcher_for = []<typename... P> [[nodiscard]] (P&& ... p) {
    return matcher_t<S, P...>{ std::forward<P>(p)... };
};
} // namespace sstd
} // namespace sstd
} // namespace hycc
