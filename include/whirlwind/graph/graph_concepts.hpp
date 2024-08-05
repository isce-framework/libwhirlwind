#pragma once

#include <concepts>

#include <whirlwind/common/namespace.hpp>

WHIRLWIND_NAMESPACE_BEGIN

namespace detail {

template<class Graph, class Vertex, class Edge, class Size>
concept GraphTypeImpl = requires(const Graph g, const Vertex v, const Edge e) {
    requires std::same_as<Size, std::size_t>;

    { g.num_vertices() } -> std::common_reference_with<Size>;
    { g.num_edges() } -> std::common_reference_with<Size>;
    { g.get_vertex_id(v) } -> std::common_reference_with<Size>;
    { g.get_edge_id(e) } -> std::common_reference_with<Size>;

    g.vertices();
    g.edges();
    g.outgoing_edges(v);
};

} // namespace detail

template<class T>
concept GraphType = detail::GraphTypeImpl<T,
                                          typename T::vertex_type,
                                          typename T::edge_type,
                                          typename T::size_type>;

WHIRLWIND_NAMESPACE_END
