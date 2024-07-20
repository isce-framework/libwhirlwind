#pragma once

#include <concepts>
#include <limits>
#include <type_traits>

#include "compatibility.hpp"
#include "namespace.hpp"

WHIRLWIND_NAMESPACE_BEGIN

template<class T>
concept Numeric = std::is_arithmetic_v<T>;

template<Numeric Number>
[[nodiscard]] WHIRLWIND_CONSTEVAL auto
zero() noexcept -> Number
{
    return static_cast<Number>(0);
}

template<Numeric Number>
[[nodiscard]] WHIRLWIND_CONSTEVAL auto
one() noexcept -> Number
{
    return static_cast<Number>(1);
}

/** The mathematical constant $\pi$. */
template<std::floating_point Real>
[[nodiscard]] WHIRLWIND_CONSTEVAL auto
pi() noexcept -> Real
{
    return static_cast<Real>(3.141592653589793238462643383279502884L);
}

/** The mathematical constant $\tau = 2\pi$. */
template<std::floating_point Real>
[[nodiscard]] WHIRLWIND_CONSTEVAL auto
tau() noexcept -> Real
{
    return static_cast<Real>(6.28318530717958647692528676655900576L);
}

template<Numeric Number>
[[nodiscard]] WHIRLWIND_CONSTEVAL auto
infinity() noexcept -> Number
{
    using T = std::numeric_limits<Number>;
    if constexpr (T::has_infinity) {
        return T::infinity();
    }
    return T::max();
}

WHIRLWIND_NAMESPACE_END
