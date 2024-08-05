#pragma once

#include <concepts>

#include <whirlwind/common/namespace.hpp>

WHIRLWIND_NAMESPACE_BEGIN

namespace detail {

template<class Forest, class Graph, class Vertex, class Edge, class Pred>
concept ForestTypeImpl = requires(const Forest f, const Vertex v) {
    requires std::same_as<Pred, std::pair<Vertex, Edge>>;

    { f.graph() } -> std::common_reference_with<Graph>;
    { f.predecessor_vertex(v) } -> std::common_reference_with<Vertex>;
    { f.predecessor_edge(v) } -> std::common_reference_with<Edge>;
    { f.predecessor(v) } -> std::common_reference_with<Pred>;
    { f.is_root_vertex(v) } -> std::convertible_to<bool>;

    f.predecessors(v);
};

template<class Forest, class Vertex, class Edge, class Pred>
concept MutableForestTypeImpl =
        requires(Forest f, const Vertex v, const Edge e, const Pred p) {
            f.set_predecessor(v, v, e);
            f.set_predecessor(v, p);
            f.make_root_vertex(v);
            f.reset();
        };

template<class ShortestPathForest, class Vertex, class Distance>
concept ShortestPathForestTypeImpl =
        requires(const ShortestPathForest f, const Vertex v) {
            { f.has_reached_vertex(v) } -> std::convertible_to<bool>;
            { f.has_visited_vertex(v) } -> std::convertible_to<bool>;
            { f.distance_to_vertex(v) } -> std::common_reference_with<Distance>;

            f.reached_vertices();
            f.visited_vertices();
        };

template<class ShortestPathForest, class Vertex, class Distance>
concept MutableShortestPathForestTypeImpl =
        requires(ShortestPathForest f, const Vertex v, const Distance d) {
            f.label_vertex_reached(v);
            f.label_vertex_visited(v);
            f.set_distance_to_vertex(v, d);
            f.reset();
        };

} // namespace detail

template<class T>
concept ForestType = detail::ForestTypeImpl<T,
                                            typename T::graph_type,
                                            typename T::vertex_type,
                                            typename T::edge_type,
                                            typename T::pred_type>;

template<class T>
concept MutableForestType =
        ForestType<T> && detail::MutableForestTypeImpl<T,
                                                       typename T::vertex_type,
                                                       typename T::edge_type,
                                                       typename T::pred_type>;

template<class T>
concept ShortestPathForestType =
        ForestType<T> && detail::ShortestPathForestTypeImpl<T,
                                                            typename T::vertex_type,
                                                            typename T::distance_type>;
template<class T>
concept MutableShortestPathForestType =
        ShortestPathForestType<T> && MutableForestType<T> &&
        detail::MutableShortestPathForestTypeImpl<T,
                                                  typename T::vertex_type,
                                                  typename T::distance_type>;

WHIRLWIND_NAMESPACE_END
