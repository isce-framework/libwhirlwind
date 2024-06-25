#pragma once

#include <cmath>
#include <cstdint>
#include <type_traits>
#include <vector>

#include "common/assert.hpp"
#include "common/namespace.hpp"
#include "common/ndarray.hpp"
#include "common/numbers.hpp"

WHIRLWIND_NAMESPACE_BEGIN

template<class SignedInteger = std::int32_t,
         template<class> class Container = std::vector,
         class ArrayLike2D>
[[nodiscard]] constexpr auto
residue(const ArrayLike2D& wrapped_phase)
        -> Array2D<SignedInteger, Container<SignedInteger>>
{
    WHIRLWIND_STATIC_ASSERT(std::is_signed_v<SignedInteger> &&
                            std::is_integral_v<SignedInteger>);
    using Extents = typename ArrayLike2D::extents_type;
    WHIRLWIND_STATIC_ASSERT(Extents::rank() == 2);

    const auto m = wrapped_phase.extent(0);
    const auto n = wrapped_phase.extent(1);
    WHIRLWIND_ASSERT(m >= 1);
    WHIRLWIND_ASSERT(n >= 1);
    auto out = Array2D<SignedInteger, Container<SignedInteger>>(m + 1, n + 1);

    // Checks whether the argument is in the interval [-pi, pi].
    [[maybe_unused]] auto is_wrapped_phase = [](const auto& psi) {
        using T = decltype(psi);
        return (psi >= -pi_v<T>) && (psi <= pi_v<T>);
    };

    auto cycle_diff_residual = [](const auto& a, const auto& b) -> SignedInteger {
        const auto diff = a - b;
        return static_cast<SignedInteger>(std::round(diff / tau_v<decltype(diff)>));
    };

    using Index = std::remove_const_t<decltype(m)>;
    for (Index i = 0; i < m - 1; ++i) {
        for (Index j = 0; j < n - 1; ++j) {
            const auto phi_00 = wrapped_phase(i, j);
            const auto phi_10 = wrapped_phase(i + 1, j);
            const auto phi_01 = wrapped_phase(i, j + 1);
            WHIRLWIND_ASSERT(is_wrapped_phase(phi_00));
            WHIRLWIND_ASSERT(is_wrapped_phase(phi_10));
            WHIRLWIND_ASSERT(is_wrapped_phase(phi_01));

            const auto di = cycle_diff_residual(phi_00, phi_10);
            const auto dj = cycle_diff_residual(phi_01, phi_00);
            WHIRLWIND_DEBUG_ASSERT((di == -1) || (di == 0) || (di == 1));
            WHIRLWIND_DEBUG_ASSERT((dj == -1) || (dj == 0) || (dj == 1));

            WHIRLWIND_DEBUG_ASSERT(i + 1 < out.extent(0));
            WHIRLWIND_DEBUG_ASSERT(j + 1 < out.extent(1));
            out(i + 1, j) += di;
            out(i, j + 1) += dj;
            out(i + 1, j + 1) -= di + dj;
        }
    }

    // Last column.
    for (Index i = 0, j = n - 1; i < m - 1; ++i) {
        const auto phi0 = wrapped_phase(i, j);
        const auto phi1 = wrapped_phase(i + 1, j);
        WHIRLWIND_ASSERT(is_wrapped_phase(phi0));
        WHIRLWIND_ASSERT(is_wrapped_phase(phi1));

        const auto d = cycle_diff_residual(phi0, phi1);
        WHIRLWIND_DEBUG_ASSERT((d == -1) || (d == 0) || (d == 1));

        WHIRLWIND_DEBUG_ASSERT(i + 1 < out.extent(0));
        WHIRLWIND_DEBUG_ASSERT(j + 1 < out.extent(1));
        out(i + 1, j) += d;
        out(i + 1, j + 1) -= d;
    }

    // Last row.
    for (Index i = m - 1, j = 0; j < n - 1; ++j) {
        const auto phi0 = wrapped_phase(i, j);
        const auto phi1 = wrapped_phase(i, j + 1);
        WHIRLWIND_ASSERT(is_wrapped_phase(phi0));
        WHIRLWIND_ASSERT(is_wrapped_phase(phi1));

        const auto d = cycle_diff_residual(phi1, phi0);
        WHIRLWIND_DEBUG_ASSERT((d == -1) || (d == 0) || (d == 1));

        WHIRLWIND_DEBUG_ASSERT(i + 1 < out.extent(0));
        WHIRLWIND_DEBUG_ASSERT(j + 1 < out.extent(1));
        out(i, j + 1) += d;
        out(i + 1, j + 1) -= d;
    }

    return out;
}

WHIRLWIND_NAMESPACE_END
