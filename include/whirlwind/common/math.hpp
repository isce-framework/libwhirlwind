#pragma once

#include <concepts>

#include "namespace.hpp"

WHIRLWIND_NAMESPACE_BEGIN

template<std::integral Integer>
[[nodiscard]] constexpr auto
is_even(Integer i) noexcept -> bool
{
    return i % static_cast<Integer>(2) == static_cast<Integer>(0);
}

WHIRLWIND_NAMESPACE_END
