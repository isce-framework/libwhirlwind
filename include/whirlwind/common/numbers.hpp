#pragma once

#include <concepts>

#include "compatibility.hpp"
#include "namespace.hpp"

WHIRLWIND_NAMESPACE_BEGIN

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

WHIRLWIND_NAMESPACE_END
