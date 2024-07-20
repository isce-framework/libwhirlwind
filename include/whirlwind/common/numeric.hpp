#pragma once

#include "namespace.hpp"

WHIRLWIND_NAMESPACE_BEGIN

template<class Integer>
[[nodiscard]] constexpr auto
is_even(Integer i) noexcept -> bool
{
    return i % Integer(2) == Integer(0);
}

WHIRLWIND_NAMESPACE_END
