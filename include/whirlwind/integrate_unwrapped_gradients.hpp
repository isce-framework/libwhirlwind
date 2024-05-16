#pragma once

#include <cmath>
#include <vector>

#include "common/namespace.hpp"
#include "common/ndarray.hpp"
#include "common/numbers.hpp"
#include "common/type_traits.hpp"
#include "graph/rectangular_grid_graph.hpp"
#include "network/network.hpp"

WHIRLWIND_NAMESPACE_BEGIN

template<template<class> class Container = std::vector,
         class ArrayLike2D,
         class Dim,
         class Cost,
         class Flow,
         // clang-format off
         template<class> class ContainerU,
         // clang-format on
         class Mixin>
[[nodiscard]] constexpr auto
integrate_unwrapped_gradients(
        const ArrayLike2D& wrapped_phase,
        const Network<RectangularGridGraph<1, Dim>, Cost, Flow, ContainerU, Mixin>&
                network)
{
    using Extents = typename ArrayLike2D::extents_type;
    WHIRLWIND_STATIC_ASSERT(Extents::rank() == 2);

    const auto m = wrapped_phase.extent(0);
    const auto n = wrapped_phase.extent(1);
    WHIRLWIND_ASSERT(m >= 1);
    WHIRLWIND_ASSERT(n >= 1);

    const auto& residual_graph = network.residual_graph();
    WHIRLWIND_ASSERT(residual_graph.num_rows() == m + 1);
    WHIRLWIND_ASSERT(residual_graph.num_cols() == n + 1);

    using Real = remove_cvref_t<typename ArrayLike2D::value_type>;
    auto unwrapped_phase = Array2D<Real, Container<Real>>(m, n);

    auto wrapped_diff = [](const auto& a, const auto& b) {
        const auto diff = b - a;
        return diff - tau_v<decltype(diff)> * std::round(diff / tau_v<decltype(diff)>);
    };

    using ResidualGraph = remove_cvref_t<decltype(residual_graph)>;
    using Vertex = ResidualGraph::vertex_type;

    unwrapped_phase(0, 0) = wrapped_phase(0, 0);

    using Index = std::remove_const_t<decltype(m)>;
    for (Index i = 1; i < m; ++i) {
        const auto phi0 = unwrapped_phase(i - 1, 0);

        const auto psi0 = wrapped_phase(i - 1, 0);
        const auto psi1 = wrapped_phase(i, 0);
        const auto dpsi = wrapped_diff(psi0, psi1);

        const auto node = Vertex(i, 0);
        WHIRLWIND_DEBUG_ASSERT(residual_graph.contains_vertex(node));
        const auto arc = residual_graph.get_down_edge(node);
        WHIRLWIND_DEBUG_ASSERT(residual_graph.contains_edge(arc));
        const auto x = network.arc_flow(arc);

        unwrapped_phase(i, 0) = phi0 + dpsi + tau_v<decltype(dpsi)> * x;
    }

    for (Index i = 0; i < m; ++i) {
        for (Index j = 1; j < n; ++j) {
            const auto phi0 = unwrapped_phase(i, j - 1);

            const auto psi0 = wrapped_phase(i, j - 1);
            const auto psi1 = wrapped_phase(i, j);
            const auto dpsi = wrapped_diff(psi0, psi1);

            const auto node = Vertex(i, j);
            WHIRLWIND_DEBUG_ASSERT(residual_graph.contains_vertex(node));
            const auto arc = residual_graph.get_right_edge(node);
            WHIRLWIND_DEBUG_ASSERT(residual_graph.contains_edge(arc));
            const auto x = network.arc_flow(arc);

            unwrapped_phase(i, j) = phi0 + dpsi + tau_v<decltype(dpsi)> * x;
        }
    }

    return unwrapped_phase;
}

WHIRLWIND_NAMESPACE_END
