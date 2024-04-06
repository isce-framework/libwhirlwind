#pragma once

#include <utility>

#include <whirlwind/common/namespace.hpp>
#include <whirlwind/common/type_traits.hpp>

WHIRLWIND_NAMESPACE_BEGIN

template<class Graph>
struct GraphTraits {
    using graph_type = Graph;
    using vertex_type = typename graph_type::vertex_type;
    using edge_type = typename graph_type::edge_type;
    using size_type = typename graph_type::size_type;
    using vertices_type =
            remove_cvref_t<decltype(std::declval<graph_type>().vertices())>;
    using edges_type = remove_cvref_t<decltype(std::declval<graph_type>().edges())>;
    using outgoing_edges_type =
            remove_cvref_t<decltype(std::declval<graph_type>().outgoing_edges(
                    std::declval<vertex_type>()))>;
};

WHIRLWIND_NAMESPACE_END
