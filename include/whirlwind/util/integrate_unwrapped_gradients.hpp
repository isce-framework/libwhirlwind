#pragma once

#include <cmath>
#include <type_traits>

#include "array/ndarray.hpp"
#include "common/namespace.hpp"
#include "container/vector.hpp"
#include "graph/rectangular_grid_graph.hpp"
#include "math/numbers.hpp"
#include "network/network.hpp"

WHIRLWIND_NAMESPACE_BEGIN

template<template<class> class Container = Vector,
         class Accumulator = double,
         class ArrayLike2D,
         class Dim,
         class Cost,
         class Flow,
         // clang-format off
         template<class> class UContainer,
         // clang-format on
         class Mixin>
[[nodiscard]] constexpr auto
integrate_unwrapped_gradients(
        const ArrayLike2D& wrapped_phase,
        const Network<RectangularGridGraph<1, Dim>, Cost, Flow, UContainer, Mixin>&
                network)
{
    // The input wrapped phase array must be a real-valued 2-D array.
    using Real = typename ArrayLike2D::value_type;
    WHIRLWIND_STATIC_ASSERT(std::is_floating_point_v<Real>);
    using Extents = typename ArrayLike2D::extents_type;
    WHIRLWIND_STATIC_ASSERT(Extents::rank() == 2);

    // `Flow` must be a signed type. The sign of the net flow between two nodes is used
    // to indicate the direction of flow.
    WHIRLWIND_STATIC_ASSERT(std::is_signed_v<Flow>);

    // Check that the wrapped phase array and network grid graph have compatible shapes.
    // The graph is expected to contain exactly one more row and one more column of
    // nodes than the array dimensions.
    const auto m = wrapped_phase.extent(0);
    const auto n = wrapped_phase.extent(1);
    const auto& residual_graph = network.residual_graph();
    WHIRLWIND_ASSERT(residual_graph.num_rows() == m + 1);
    WHIRLWIND_ASSERT(residual_graph.num_cols() == n + 1);

    // TODO: Check that the flow in the network is feasible

    // Initialize the output array.
    auto unwrapped_phase = Array2D<Real, Container<Real>>(m, n);

    // If the input array is Mx0 or 0xN, there's nothing to do.
    if ((m == 0) || (n == 0)) {
        return unwrapped_phase;
    }

    // Checks whether the argument is in the interval [-pi, pi].
    [[maybe_unused]] auto is_wrapped_phase = [](const auto& psi) {
        using T = decltype(psi);
        return (psi >= -pi<T>()) && (psi <= pi<T>());
    };

    // Computes the difference between the two input phase values (in radians), wrapped
    // to the interval [-pi, pi).
    auto wrapped_diff = [](const auto& a, const auto& b) {
        const auto diff = a - b;
        using T = decltype(diff);
        return diff - tau<T>() * std::round(diff / tau<T>());
    };

    using ResidualGraph = std::remove_cvref_t<decltype(residual_graph)>;
    using Vertex = ResidualGraph::vertex_type;

    // Start with a fixed "seed" point where the wrapped and unwrapped phase values are
    // forced to be equal.
    unwrapped_phase(0, 0) = wrapped_phase(0, 0);

    // Scan down the first column. Accumulate the unwrapped phase gradients between each
    // adjacent pair of pixels to get the unwrapped phase values.
    auto phi = Accumulator{unwrapped_phase(0, 0)};
    using Index = std::remove_const_t<decltype(m)>;
    for (Index i = 1; i < m; ++i) {
        // Compute the wrapped phase gradient between the pair of adjacent phase values.
        const auto psi0 = wrapped_phase(i - 1, 0);
        const auto psi1 = wrapped_phase(i, 0);
        WHIRLWIND_ASSERT(is_wrapped_phase(psi0));
        WHIRLWIND_ASSERT(is_wrapped_phase(psi1));
        const auto dpsi = wrapped_diff(psi1, psi0);
        WHIRLWIND_DEBUG_ASSERT(is_wrapped_phase(dpsi));

        // Get the two residues (nodes) in the network that both border the edge between
        // the pair of pixels.
        WHIRLWIND_DEBUG_ASSERT(i < residual_graph.num_rows());
        WHIRLWIND_DEBUG_ASSERT(1 < residual_graph.num_cols());
        const auto node0 = Vertex(i, 0);
        const auto node1 = Vertex(i, 1);
        WHIRLWIND_DEBUG_ASSERT(residual_graph.contains_vertex(node0));
        WHIRLWIND_DEBUG_ASSERT(residual_graph.contains_vertex(node1));

        // Get the net leftward flow between the two neighboring residues. If the
        // residues were formed from clockwise loops, this corresponds to the difference
        // (in cycles) between the unwrapped & wrapped phase gradients in the downward
        // direction (from the upper to the lower pixel).
        const auto arc0 = residual_graph.get_right_edge(node0);
        const auto arc1 = residual_graph.get_left_edge(node1);
        WHIRLWIND_DEBUG_ASSERT(residual_graph.contains_edge(arc0));
        WHIRLWIND_DEBUG_ASSERT(residual_graph.contains_edge(arc1));
        const auto net_flow = network.arc_flow(arc1) - network.arc_flow(arc0);

        // Get the unwrapped phase gradient between the pixels and add it to the
        // cumulative sum.
        const auto dphi = dpsi + tau<decltype(dpsi)>() * net_flow;
        phi += Accumulator{dphi};

        // Store the unwrapped phase value.
        unwrapped_phase(i, 0) = static_cast<Real>(phi);
    }

    // Scan across each row. Accumulate the unwrapped phase gradients between each
    // adjacent pair of pixels to get the unwrapped phase values.
    for (Index i = 0; i < m; ++i) {
        auto phi = Accumulator{unwrapped_phase(i, 0)};

        for (Index j = 1; j < n; ++j) {
            // Compute the wrapped phase gradient between the pair of adjacent phase
            // values.
            const auto psi0 = wrapped_phase(i, j - 1);
            const auto psi1 = wrapped_phase(i, j);
            WHIRLWIND_ASSERT(is_wrapped_phase(psi0));
            WHIRLWIND_ASSERT(is_wrapped_phase(psi1));
            const auto dpsi = wrapped_diff(psi1, psi0);
            WHIRLWIND_DEBUG_ASSERT(is_wrapped_phase(dpsi));

            // Get the two residues (nodes) in the network that both border the edge
            // between the pair of pixels.
            WHIRLWIND_DEBUG_ASSERT(i + 1 < residual_graph.num_rows());
            WHIRLWIND_DEBUG_ASSERT(j < residual_graph.num_cols());
            const auto node0 = Vertex(i, j);
            const auto node1 = Vertex(i + 1, j);
            WHIRLWIND_DEBUG_ASSERT(residual_graph.contains_vertex(node0));
            WHIRLWIND_DEBUG_ASSERT(residual_graph.contains_vertex(node1));

            // Get the net downward flow between the two neighboring residues. If the
            // residues were formed from clockwise loops, this corresponds to the
            // difference (in cycles) between the unwrapped & wrapped phase gradients in
            // the rightward direction (from the left to the right pixel).
            const auto arc0 = residual_graph.get_down_edge(node0);
            const auto arc1 = residual_graph.get_up_edge(node1);
            WHIRLWIND_DEBUG_ASSERT(residual_graph.contains_edge(arc0));
            WHIRLWIND_DEBUG_ASSERT(residual_graph.contains_edge(arc1));
            const auto net_flow = network.arc_flow(arc0) - network.arc_flow(arc1);

            // Get the unwrapped phase gradient between the pixels and add it to the
            // cumulative sum.
            const auto dphi = dpsi + tau<decltype(dpsi)>() * net_flow;
            phi += Accumulator{dphi};

            // Store the unwrapped phase value.
            unwrapped_phase(i, j) = static_cast<Real>(phi);
        }
    }

    return unwrapped_phase;
}

WHIRLWIND_NAMESPACE_END
