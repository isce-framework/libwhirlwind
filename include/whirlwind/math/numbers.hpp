#pragma once

#include <concepts>
#include <limits>
#include <type_traits>

#include <whirlwind/common/compatibility.hpp>
#include <whirlwind/common/namespace.hpp>

WHIRLWIND_NAMESPACE_BEGIN

/** An integral or floating-point type (or cv-qualified version thereof). */
template<class T>
concept Numeric = std::is_arithmetic_v<T>;

/**
 * Return the special value zero.
 *
 * @tparam Number
 *     A numeric (integral or floating-point) type.
 *
 * @returns
 *     ``static_cast<Number>(0)``.
 */
template<Numeric Number>
[[nodiscard]] WHIRLWIND_CONSTEVAL auto
zero() noexcept -> Number
{
    return static_cast<Number>(0);
}

/**
 * Return the special value one.
 *
 * @tparam Number
 *     A numeric (integral or floating-point) type.
 *
 * @returns
 *     ``static_cast<Number>(1)``.
 */
template<Numeric Number>
[[nodiscard]] WHIRLWIND_CONSTEVAL auto
one() noexcept -> Number
{
    return static_cast<Number>(1);
}

/**
 * Return the mathematical constant $\pi$.
 *
 * @tparam Real
 *     A floating-point type.
 *
 * @returns
 *     The mathematical constant $\pi$.
 */
template<std::floating_point Real>
[[nodiscard]] WHIRLWIND_CONSTEVAL auto
pi() noexcept -> Real
{
    return static_cast<Real>(3.141592653589793238462643383279502884L);
}

/**
 * Return the mathematical constant $\tau = 2\pi$.
 *
 * @tparam Real
 *     A floating-point type.
 *
 * @returns
 *     The mathematical constant $\tau = 2\pi$.
 */
template<std::floating_point Real>
[[nodiscard]] WHIRLWIND_CONSTEVAL auto
tau() noexcept -> Real
{
    return static_cast<Real>(6.28318530717958647692528676655900576L);
}

/**
 * Return positive infinity (or a large number).
 *
 * Returns positive infinity if it can represented by `Number` (as determined by
 * ``std::numeric_limits<Number>::has_infinity``). Otherwise, returns the largest finite
 * value representable by `Number`.
 *
 * @tparam Number
 *     A numeric (integral or floating-point) type.
 *
 * @returns
 *     Positive infinity if representable by `Number`; the maximum finite representable
 *     value of `Number` otherwise.
 */
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
