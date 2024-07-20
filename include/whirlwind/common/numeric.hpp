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
