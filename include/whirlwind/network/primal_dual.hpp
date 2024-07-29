#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

#include <range/v3/algorithm/fill.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/algorithm/unique.hpp>
#include <range/v3/range/conversion.hpp>

#include <whirlwind/common/assert.hpp>
#include <whirlwind/common/namespace.hpp>
#include <whirlwind/container/vector.hpp>
#include <whirlwind/logging/null_logger.hpp>
#include <whirlwind/math/numbers.hpp>

#include "successive_shortest_paths.hpp"

WHIRLWIND_NAMESPACE_BEGIN

template<class Dijkstra>
class PrimalDualDijkstra : public Dijkstra {
private:
    using super_type = Dijkstra;

public:
    using graph_type = typename super_type::graph_type;
    using vertex_type = typename super_type::vertex_type;
    using edge_type = typename super_type::edge_type;
    using pred_type = typename super_type::pred_type;
    using distance_type = typename super_type::distance_type;

    template<class T>
    using container_type = typename super_type::template container_type<T>;

    using super_type::distance_to_vertex;
    using super_type::graph;
    using super_type::predecessor_vertex;

    template<class Network>
    explicit constexpr PrimalDualDijkstra(const Network& network,
                                          vertex_type source_fill_value = {})
        : super_type(network),
          source_(network.residual_graph().num_vertices(), source_fill_value),
          source_fill_value_(std::move(source_fill_value))
    {
        WHIRLWIND_DEBUG_ASSERT(std::size(source_) == this->graph().num_vertices());
    }

    constexpr void
    set_source_vertex(const vertex_type& vertex, vertex_type source)
    {
        WHIRLWIND_ASSERT(graph().contains_vertex(vertex));
        WHIRLWIND_ASSERT(graph().contains_vertex(source));
        WHIRLWIND_DEBUG_ASSERT(predecessor_vertex(source) == source);
        const auto vertex_id = graph().get_vertex_id(vertex);
        WHIRLWIND_DEBUG_ASSERT(vertex_id < std::size(source_));
        source_[vertex_id] = std::move(source);
    }

    [[nodiscard]] constexpr auto
    source_vertex(const vertex_type& vertex) const -> const vertex_type&
    {
        WHIRLWIND_ASSERT(graph().contains_vertex(vertex));
        const auto vertex_id = graph().get_vertex_id(vertex);
        WHIRLWIND_DEBUG_ASSERT(vertex_id < std::size(source_));
        return source_[vertex_id];
    }

    constexpr void
    add_source(const vertex_type& source)
    {
        super_type::add_source(source);
        set_source_vertex(source, source);
    }

    constexpr void
    reach_vertex(edge_type edge,
                 vertex_type tail,
                 vertex_type head,
                 distance_type distance)
    {
        super_type::reach_vertex(std::move(edge), tail, head, std::move(distance));
        set_source_vertex(head, source_vertex(tail));
    }

    constexpr void
    relax_edge(edge_type edge,
               vertex_type tail,
               vertex_type head,
               distance_type distance)
    {
        WHIRLWIND_ASSERT(graph().contains_edge(edge));
        WHIRLWIND_ASSERT(graph().contains_vertex(tail));
        WHIRLWIND_ASSERT(graph().contains_vertex(head));
        WHIRLWIND_ASSERT(distance >= zero<distance_type>());

        if (distance < distance_to_vertex(head)) {
            reach_vertex(std::move(edge), std::move(tail), std::move(head),
                         std::move(distance));
        }
    }

    [[nodiscard]] constexpr auto
    source_fill_value() const noexcept -> const vertex_type&
    {
        return source_fill_value_;
    }

    constexpr void
    reset()
    {
        super_type::reset();
        ranges::fill(source_, source_fill_value());
    }

private:
    container_type<vertex_type> source_;
    vertex_type source_fill_value_;
};

template<class Network>
[[nodiscard]] constexpr auto
contains_any_excess_node(const Network& network) -> bool
{
    for ([[maybe_unused]] const auto& node : network.excess_nodes()) {
        return true;
    }
    return false;
}

// Find the shortest path w.r.t the reduced arc costs to each node from any excess node
// using Dijkstra's algorithm.
template<class Dijkstra, class Network>
constexpr void
dijkstra_pd(Dijkstra& dijkstra, const Network& network)
{
    using Distance = typename Dijkstra::distance_type;
    WHIRLWIND_STATIC_ASSERT(std::is_same_v<Distance, typename Network::cost_type>);

    WHIRLWIND_ASSERT(std::addressof(dijkstra.graph()) ==
                     std::addressof(network.residual_graph()));

    for (const auto& source : network.excess_nodes()) {
        dijkstra.add_source(source);
        WHIRLWIND_DEBUG_ASSERT(dijkstra.has_reached_vertex(source));
        WHIRLWIND_DEBUG_ASSERT(dijkstra.distance_to_vertex(source) == zero<Distance>());
    }

    while (!dijkstra.done()) {
        const auto [tail, distance] = dijkstra.pop_next_unvisited_vertex();
        WHIRLWIND_DEBUG_ASSERT(network.contains_node(tail));
        WHIRLWIND_DEBUG_ASSERT(distance >= zero<Distance>());

        dijkstra.visit_vertex(tail, distance);
        WHIRLWIND_DEBUG_ASSERT(dijkstra.has_visited_vertex(tail));
        WHIRLWIND_DEBUG_ASSERT(dijkstra.distance_to_vertex(tail) == distance);

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
}

template<template<class> class Container = Vector, class Network, class Dijkstra>
constexpr void
augment_flow_pd(Network& network, const Dijkstra& dijkstra)
{
    WHIRLWIND_ASSERT(std::addressof(network.residual_graph()) ==
                     std::addressof(dijkstra.graph()));

    using Node = typename Network::node_type;
    auto sinks = network.deficit_nodes() | ranges::to<Container<Node>>();
    ranges::sort(sinks, [&](const auto& lhs, const auto& rhs) {
        const auto lhs_source = dijkstra.source_vertex(lhs);
        const auto rhs_source = dijkstra.source_vertex(rhs);
        WHIRLWIND_DEBUG_ASSERT(network.contains_node(lhs_source));
        WHIRLWIND_DEBUG_ASSERT(network.contains_node(rhs_source));

        const auto lhs_source_id = network.get_node_id(lhs_source);
        const auto rhs_source_id = network.get_node_id(rhs_source);

        if (lhs_source_id == rhs_source_id) {
            const auto lhs_distance = dijkstra.distance_to_vertex(lhs);
            const auto rhs_distance = dijkstra.distance_to_vertex(rhs);
            return lhs_distance < rhs_distance;
        }

        return lhs_source_id < rhs_source_id;
    });
    auto it = ranges::unique(sinks, {}, [&](const auto& sink) {
        const auto source = dijkstra.source_vertex(sink);
        WHIRLWIND_DEBUG_ASSERT(network.contains_node(source));
        return network.get_node_id(source);
    });
    WHIRLWIND_DEBUG_ASSERT(std::distance(it, std::end(sinks)) >= 0);
    sinks.erase(it, std::end(sinks));

    using Flow = typename Network::flow_type;
    constexpr auto delta = one<Flow>();

    for (const auto& sink : sinks) {
        WHIRLWIND_DEBUG_ASSERT(network.is_deficit_node(sink));
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
}

template<class Network, class Dijkstra>
constexpr void
update_potential_pd(Network& network, const Dijkstra& dijkstra)
{
    using Distance = typename Dijkstra::distance_type;
    WHIRLWIND_STATIC_ASSERT(std::is_same_v<Distance, typename Network::cost_type>);

    WHIRLWIND_ASSERT(std::addressof(network.residual_graph()) ==
                     std::addressof(dijkstra.graph()));

    for (const auto& node : network.nodes()) {
        WHIRLWIND_DEBUG_ASSERT(dijkstra.has_visited_vertex(node));
        const auto distance = dijkstra.distance_to_vertex(node);
        WHIRLWIND_DEBUG_ASSERT(distance >= zero<Distance>());
        network.decrease_node_potential(node, distance);
        WHIRLWIND_DEBUG_ASSERT(network.node_potential(node) <= zero<Distance>());
    }
}

template<class Dijkstra, class Logger = NullLogger, class Network>
constexpr void
primal_dual(Network& network, std::size_t maxiter = 0)
{
    auto logger = Logger("whirlwind.network.primal_dual");

    WHIRLWIND_ASSERT(network.is_balanced());

    std::size_t iter = 1;
    while (true) {
        logger.info("Iteration {}", iter);

        auto dijkstra = PrimalDualDijkstra<Dijkstra>(network);
        WHIRLWIND_DEBUG_ASSERT(std::addressof(dijkstra.graph()) ==
                               std::addressof(network.residual_graph()));

        dijkstra_pd(dijkstra, network);
        augment_flow_pd(network, dijkstra);

        if (!contains_any_excess_node(network)) {
            return;
        }

        update_potential_pd(network, dijkstra);

        if (iter == maxiter) {
            break;
        }

        ++iter;
    }

    successive_shortest_paths<Dijkstra, Logger>(network);
}

WHIRLWIND_NAMESPACE_END
