#pragma once

#include <concepts>

#include <whirlwind/common/namespace.hpp>

WHIRLWIND_NAMESPACE_BEGIN

/**
 * Check whether the argument is even-valued.
 *
 * @tparam Integer
 *     An integral type.
 *
 * @param[in] i
 *     The input value.
 *
 * @returns
 *     True if `i` is even-valued; false otherwise.
 */
template<std::integral Integer>
[[nodiscard]] constexpr auto
is_even(Integer i) noexcept -> bool
{
    return i % static_cast<Integer>(2) == static_cast<Integer>(0);
}

WHIRLWIND_NAMESPACE_END
