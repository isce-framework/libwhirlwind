#pragma once

#include <concepts>

#include <whirlwind/common/namespace.hpp>

WHIRLWIND_NAMESPACE_BEGIN

namespace detail {

template<class Forest, class Graph, class Vertex, class Edge, class Pred>
concept ForestWith = requires(const Forest forest, Vertex vertex, Edge) {
    requires std::same_as<Pred, std::pair<Vertex, Edge>>;

    { forest.graph() } -> std::same_as<const Graph&>;
    { forest.predecessor_vertex(vertex) } -> std::convertible_to<Vertex>;
    { forest.predecessor_edge(vertex) } -> std::convertible_to<Edge>;
    { forest.predecessor(vertex) } -> std::convertible_to<Pred>;
    { forest.is_root_vertex(vertex) } -> std::convertible_to<bool>;

    forest.predecessors(vertex);
};

template<class Forest, class Vertex, class Edge, class Pred>
concept MutableForestWith =
        requires(Forest forest, Vertex vertex, Edge edge, Pred pred) {
            forest.set_predecessor(vertex, vertex, edge);
            forest.set_predecessor(vertex, pred);
            forest.make_root_vertex(vertex);
            forest.reset();
        };

} // namespace detail

template<class F>
concept Forest = detail::ForestWith<F,
                                    typename F::graph_type,
                                    typename F::vertex_type,
                                    typename F::edge_type,
                                    typename F::pred_type>;

template<class F>
concept MutableForest = Forest<F> && detail::MutableForestWith<F,
                                                               typename F::vertex_type,
                                                               typename F::edge_type,
                                                               typename F::pred_type>;

WHIRLWIND_NAMESPACE_END
