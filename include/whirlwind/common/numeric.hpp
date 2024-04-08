#pragma once

#include <limits>

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
infinity() noexcept -> Numeric
{
    using T = std::numeric_limits<Numeric>;
    if constexpr (T::has_infinity) {
        return T::infinity();
    }
    return T::max();
}

WHIRLWIND_NAMESPACE_END
