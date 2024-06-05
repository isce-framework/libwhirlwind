#pragma once

#include <limits>
#include <type_traits>

#include "assert.hpp"
#include "compatibility.hpp"
#include "namespace.hpp"

WHIRLWIND_NAMESPACE_BEGIN

template<class Numeric>
[[nodiscard]] WHIRLWIND_CONSTEVAL auto
zero() noexcept -> Numeric
{
    return static_cast<Numeric>(0);
}

template<class Numeric>
[[nodiscard]] WHIRLWIND_CONSTEVAL auto
one() noexcept -> Numeric
{
    return static_cast<Numeric>(1);
}

template<class Numeric>
[[nodiscard]] WHIRLWIND_CONSTEVAL auto
eps() noexcept -> Numeric
{
    WHIRLWIND_STATIC_ASSERT(std::is_integral_v<Numeric>);
    return zero<Numeric>();
}

template<>
[[nodiscard]] WHIRLWIND_CONSTEVAL auto
eps<float>() noexcept -> float
{
    return 1e-3f;
}

template<>
[[nodiscard]] WHIRLWIND_CONSTEVAL auto
eps<double>() noexcept -> double
{
    return 1e-8;
}

template<class Numeric>
[[nodiscard]] WHIRLWIND_CONSTEVAL auto
infinity() noexcept -> Numeric
{
    using T = std::numeric_limits<Numeric>;
    if constexpr (T::has_infinity) {
        return T::infinity();
    }
    return T::max();
}

template<class Integer>
[[nodiscard]] constexpr auto
is_even(Integer i) noexcept -> bool
{
    return i % Integer(2) == Integer(0);
}

WHIRLWIND_NAMESPACE_END
