#pragma once

#include <concepts>
#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

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
    constexpr ownership(ownership&& that) : block_{ that.block_ } {
        that.block_ = nullptr;
    };
    constexpr ownership& operator=(ownership&& that) {
        block_ = that.block_; 
        that.block_ = nullptr;
        return *this;
    };

    constexpr ~ownership() {
        if (block_ == nullptr) return;
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

} // namespace sstd
} // namespace hycc
