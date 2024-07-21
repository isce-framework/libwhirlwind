#pragma once

#include <concepts>

#include <whirlwind/common/namespace.hpp>

WHIRLWIND_NAMESPACE_BEGIN

namespace detail {

template<class Graph, class Vertex, class Edge, class Size>
concept DiGraphWith = requires(const Graph graph, Vertex vertex, Edge edge, Size) {
    requires std::same_as<Size, std::size_t>;

    { graph.num_vertices() } -> std::same_as<Size>;
    { graph.num_edges() } -> std::same_as<Size>;
    { graph.get_vertex_id(vertex) } -> std::same_as<Size>;
    { graph.get_edge_id(edge) } -> std::same_as<Size>;

    graph.vertices();
    graph.edges();
    graph.outgoing_edges(vertex);
};

} // namespace detail

template<class Graph>
concept DiGraph = detail::DiGraphWith<Graph,
                                      typename Graph::vertex_type,
                                      typename Graph::edge_type,
                                      typename Graph::size_type>;

WHIRLWIND_NAMESPACE_END
