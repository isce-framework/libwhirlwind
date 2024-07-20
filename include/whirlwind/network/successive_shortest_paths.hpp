#pragma once

#include <memory>
#include <optional>
#include <type_traits>

#include <whirlwind/common/assert.hpp>
#include <whirlwind/common/namespace.hpp>
#include <whirlwind/logging/null_logger.hpp>
#include <whirlwind/math/numbers.hpp>

WHIRLWIND_NAMESPACE_BEGIN

// Find the shortest path w.r.t the reduced arc costs from the source to the nearest
// deficit node using Dijkstra's algorithm.
template<class Dijkstra, class Network>
constexpr auto
dijkstra_ssp(Dijkstra& dijkstra,
             const Network& network,
             const typename Network::node_type& source)
        -> std::optional<typename Network::node_type>
{
    using Distance = typename Dijkstra::distance_type;
    WHIRLWIND_STATIC_ASSERT(std::is_same_v<Distance, typename Network::cost_type>);

    WHIRLWIND_ASSERT(network.contains_node(source));
    WHIRLWIND_ASSERT(std::addressof(dijkstra.graph()) ==
                     std::addressof(network.residual_graph()));

    dijkstra.reset();
    WHIRLWIND_DEBUG_ASSERT(dijkstra.done());
    WHIRLWIND_DEBUG_ASSERT(!dijkstra.has_reached_vertex(source));
    WHIRLWIND_DEBUG_ASSERT(dijkstra.distance_to_vertex(source) == infinity<Distance>());

    dijkstra.add_source(source);
    WHIRLWIND_DEBUG_ASSERT(!dijkstra.done());
    WHIRLWIND_DEBUG_ASSERT(dijkstra.has_reached_vertex(source));
    WHIRLWIND_DEBUG_ASSERT(dijkstra.distance_to_vertex(source) == zero<Distance>());

    while (!dijkstra.done()) {
        const auto [tail, distance] = dijkstra.pop_next_unvisited_vertex();
        WHIRLWIND_DEBUG_ASSERT(network.contains_node(tail));
        WHIRLWIND_DEBUG_ASSERT(distance >= zero<Distance>());

        dijkstra.visit_vertex(tail, distance);
        WHIRLWIND_DEBUG_ASSERT(dijkstra.has_visited_vertex(tail));
        WHIRLWIND_DEBUG_ASSERT(dijkstra.distance_to_vertex(tail) == distance);

        if (network.is_deficit_node(tail)) {
            return tail;
        }

        for (const auto& [arc, head] : network.outgoing_arcs(tail)) {
            WHIRLWIND_DEBUG_ASSERT(network.contains_arc(arc));
            WHIRLWIND_DEBUG_ASSERT(network.contains_node(head));

            if (network.is_arc_saturated(arc)) {
                continue;
            }

            const auto arc_length = network.arc_reduced_cost(arc, tail, head);
            WHIRLWIND_ASSERT(arc_length >= zero<Distance>());

            dijkstra.relax_edge(arc, tail, head, distance + arc_length);
            WHIRLWIND_DEBUG_ASSERT(dijkstra.has_reached_vertex(head));
        }
    }

    return std::nullopt;
}

template<class Network, class Dijkstra>
constexpr void
augment_flow_ssp(Network& network,
                 const Dijkstra& dijkstra,
                 const typename Network::node_type& sink)
{
    using Flow = typename Network::flow_type;

    WHIRLWIND_ASSERT(network.contains_node(sink));
    WHIRLWIND_ASSERT(dijkstra.has_visited_vertex(sink));
    WHIRLWIND_ASSERT(std::addressof(network.residual_graph()) ==
                     std::addressof(dijkstra.graph()));

    constexpr auto delta = one<Flow>();

    WHIRLWIND_ASSERT(network.is_deficit_node(sink));
    network.increase_node_excess(sink, delta);
    WHIRLWIND_ASSERT(!network.is_deficit_node(sink));

    auto head = sink;
    for (const auto& [tail, arc] : dijkstra.predecessors(sink)) {
        WHIRLWIND_DEBUG_ASSERT(network.contains_arc(arc));
        WHIRLWIND_DEBUG_ASSERT(network.contains_node(tail));
        WHIRLWIND_DEBUG_ASSERT(dijkstra.has_visited_vertex(tail));

        WHIRLWIND_DEBUG_ASSERT(network.arc_residual_capacity(arc) >= delta);
        network.increase_arc_flow(arc, delta);
        WHIRLWIND_DEBUG_ASSERT(network.arc_flow(arc) > zero<Flow>());

        head = tail;
    }

    WHIRLWIND_ASSERT(network.is_excess_node(head));
    network.decrease_node_excess(head, delta);
    WHIRLWIND_ASSERT(!network.is_excess_node(head));
}

template<class Network, class Dijkstra>
constexpr void
update_potential_ssp(Network& network,
                     const Dijkstra& dijkstra,
                     const typename Network::node_type& sink)
{
    using Distance = typename Dijkstra::distance_type;
    WHIRLWIND_STATIC_ASSERT(std::is_same_v<Distance, typename Network::cost_type>);

    WHIRLWIND_ASSERT(network.contains_node(sink));
    WHIRLWIND_ASSERT(dijkstra.has_visited_vertex(sink));
    WHIRLWIND_ASSERT(std::addressof(network.residual_graph()) ==
                     std::addressof(dijkstra.graph()));

    const auto distance_to_sink = dijkstra.distance_to_vertex(sink);
    WHIRLWIND_DEBUG_ASSERT(distance_to_sink >= zero<Distance>());

    for (const auto& node : dijkstra.visited_vertices()) {
        WHIRLWIND_DEBUG_ASSERT(network.contains_node(node));
        WHIRLWIND_DEBUG_ASSERT(dijkstra.has_visited_vertex(node));

        const auto distance = dijkstra.distance_to_vertex(node);
        WHIRLWIND_DEBUG_ASSERT(distance >= zero<Distance>());

        WHIRLWIND_DEBUG_ASSERT(distance_to_sink >= distance);
        network.increase_node_potential(node, distance_to_sink - distance);
    }
}

template<class Dijkstra, class Logger = NullLogger, class Network>
constexpr void
successive_shortest_paths(Network& network)
{
    auto logger = Logger("whirlwind.network.successive_shortest_paths");

    WHIRLWIND_ASSERT(network.is_balanced());

    auto dijkstra = Dijkstra(network);
    WHIRLWIND_DEBUG_ASSERT(dijkstra.done());
    WHIRLWIND_DEBUG_ASSERT(std::addressof(dijkstra.graph()) ==
                           std::addressof(network.residual_graph()));

    const auto num_iter = network.total_excess();
    using Iter = std::remove_const_t<decltype(num_iter)>;
    Iter iter = 1;
    for (const auto& source : network.excess_nodes()) {
        if (iter % 100 == 0) {
            logger.info("Iteration {:>8}/{}", iter, num_iter);
        }

        const auto sink = dijkstra_ssp(dijkstra, network, source);
        WHIRLWIND_ASSERT(sink);

        augment_flow_ssp(network, dijkstra, *sink);
        update_potential_ssp(network, dijkstra, *sink);

        ++iter;
    }
}

WHIRLWIND_NAMESPACE_END
