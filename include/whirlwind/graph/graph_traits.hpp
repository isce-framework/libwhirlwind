#pragma once

#include <utility>

#include <whirlwind/common/namespace.hpp>
#include <whirlwind/common/type_traits.hpp>

WHIRLWIND_NAMESPACE_BEGIN

template<class Graph>
struct GraphTraits {
    using graph_type = Graph;
    using vertex_type = typename Graph::vertex_type;
    using edge_type = typename Graph::edge_type;
    using size_type = typename Graph::size_type;
    using vertices_type = remove_cvref_t<decltype(std::declval<Graph>().vertices())>;
    using edges_type = remove_cvref_t<decltype(std::declval<Graph>().edges())>;
    using outgoing_edges_type =
            remove_cvref_t<decltype(std::declval<Graph>().outgoing_edges(
                    std::declval<vertex_type>()))>;
};

WHIRLWIND_NAMESPACE_END
