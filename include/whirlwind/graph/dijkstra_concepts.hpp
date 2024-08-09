#pragma once

#include <concepts>

#include <whirlwind/common/namespace.hpp>
#include <whirlwind/container/pair_like.hpp>

#include "graph_concepts.hpp"

WHIRLWIND_NAMESPACE_BEGIN

namespace detail {

template<class DijkstraSolver, class Distance, class Graph, class Vertex, class Edge>
concept DijkstraSolverTypeImpl =
        requires(DijkstraSolver s, const Distance d, const Vertex v, const Edge e) {
            requires GraphType<Graph>;
            requires std::same_as<Vertex, typename Graph::vertex_type>;
            requires std::same_as<Edge, typename Graph::edge_type>;

            { s.pop_next_unvisited_vertex() } -> PairLike<Vertex, Distance>;
            { s.done() } -> std::convertible_to<bool>;

            s.push_vertex(v, d);
            s.add_source(v);
            s.visit_vertex(v, d);
            s.relax_edge(e, v, v, d);
            s.reset();
        };

} // namespace detail

template<class T>
concept DijkstraSolverType = detail::DijkstraSolverTypeImpl<T,
                                                            typename T::distance_type,
                                                            typename T::graph_type,
                                                            typename T::vertex_type,
                                                            typename T::edge_type>;

WHIRLWIND_NAMESPACE_END
