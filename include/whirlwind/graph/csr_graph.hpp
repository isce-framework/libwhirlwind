#pragma once

#include <algorithm>
#include <span>
#include <type_traits>
#include <utility>

#include <range/v3/view/iota.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>

#include <whirlwind/common/assert.hpp>
#include <whirlwind/common/compatibility.hpp>
#include <whirlwind/common/namespace.hpp>
#include <whirlwind/common/stddef.hpp>
#include <whirlwind/container/vector.hpp>

#include "edge_list.hpp"

WHIRLWIND_NAMESPACE_BEGIN

/**
 * A compressed sparse row (CSR) digraph.
 *
 * Vertices and edges are represented by unsigned integers. The graph topology cannot be
 * modified after construction. Parallel edges and self-loops are supported.
 *
 * @tparam Container
 *     A `std::vector`-like type template used to store the internal row and column
 *     index arrays.
 */
template<template<class> class Container = Vector>
class CSRGraph {
public:
    using vertex_type = Size;
    using edge_type = Size;
    using size_type = Size;

    template<class T>
    using container_type = Container<T>;

    /** Default constructor. Creates an empty `CSRGraph` with no vertices or edges. */
    constexpr CSRGraph() : r_(1, 0), c_()
    {
        WHIRLWIND_DEBUG_ASSERT(num_vertices() == 0);
        WHIRLWIND_DEBUG_ASSERT(num_edges() == 0);
    }

    /** Create a new `CSRGraph` from a sequence of (tail,head) pairs. */
    template<template<class> class UContainer>
    explicit constexpr CSRGraph(EdgeList<vertex_type, UContainer> edge_list)
        : CSRGraph([&]() {
              std::sort(std::begin(edge_list), std::end(edge_list));

              CSRGraph graph;
              graph.c_.reserve(std::size(edge_list));

              size_type edge_count = 0;
              size_type max_vertex_id = 0;
              for (const auto& [tail, head] : edge_list) {
                  const auto new_size = tail + 1;
                  WHIRLWIND_DEBUG_ASSERT(new_size >= std::size(graph.r_));

                  graph.r_.resize(new_size, edge_count);
                  graph.c_.emplace_back(head);

                  max_vertex_id = std::max(max_vertex_id, std::max(tail, head));

                  ++edge_count;
              }

              const auto num_vertices = max_vertex_id + 1;
              graph.r_.resize(num_vertices + 1, edge_count);

              return graph;
          }())
    {}

    /** The total number of vertices in the graph. */
    [[nodiscard]] constexpr auto
    num_vertices() const -> size_type
    {
        WHIRLWIND_DEBUG_ASSERT(std::size(r_) >= 1);
        return size_type{std::size(r_)} - 1;
    }

    /** The total number of edges in the graph. */
    [[nodiscard]] constexpr auto
    num_edges() const -> size_type
    {
        return size_type{std::size(c_)};
    }

    /**
     * Get the unique array index of a vertex.
     *
     * Given a vertex in the graph, get the associated vertex index in the range [0, V),
     * where V is the total number of vertices.
     *
     * @param[in] vertex
     *     The input vertex. Must be a valid vertex in the graph.
     *
     * @returns
     *     The vertex index.
     */
    [[nodiscard]] constexpr auto
    get_vertex_id(const vertex_type& vertex) const noexcept -> size_type
    {
        return vertex;
    }

    /**
     * Get the unique array index of an edge.
     *
     * Given an edge in the graph, get the associated edge index in the range [0, E),
     * where E is the total number of edges.
     *
     * @param[in] edge
     *     The input edge. Must be a valid edge in the graph.
     *
     * @returns
     *     The edge index.
     */
    [[nodiscard]] constexpr auto
    get_edge_id(const edge_type& edge) const noexcept -> size_type
    {
        return edge;
    }

    /**
     * Iterate over vertices in the graph.
     *
     * Returns a view of all vertices in the graph in order from smallest index to
     * largest.
     */
    [[nodiscard]] constexpr auto
    vertices() const
    {
        return ranges::views::iota(vertex_type{0}, num_vertices());
    }

    /**
     * Iterate over edges in the graph.
     *
     * Returns a view of all edges in the graph in order from smallest index to largest.
     */
    [[nodiscard]] constexpr auto
    edges() const
    {
        return ranges::views::iota(edge_type{0}, num_edges());
    }

    /** Check whether the graph contains the specified vertex. */
    [[nodiscard]] constexpr auto
    contains_vertex(const vertex_type& vertex) const -> bool
    {
        return get_vertex_id(vertex) < num_vertices();
    }

    /** Check whether the graph contains the specified edge. */
    [[nodiscard]] constexpr auto
    contains_edge(const edge_type& edge) const -> bool
    {
        return get_edge_id(edge) < num_edges();
    }

    /**
     * Get the number of outgoing edges of a vertex.
     *
     * @param[in] vertex
     *     The input vertex. Must be a valid vertex in the graph.
     *
     * @returns
     *     The outdegree of the vertex.
     */
    [[nodiscard]] constexpr auto
    outdegree(const vertex_type& vertex) const -> size_type
    {
        WHIRLWIND_ASSERT(contains_vertex(vertex));
        const auto vertex_id = get_vertex_id(vertex);
        return r_[vertex_id + 1] - r_[vertex_id];
    }

    /**
     * Iterate over outgoing edges (and corresponding head vertices) of a vertex.
     *
     * Returns a view of ordered (edge,head) pairs over all edges emanating from the
     * specified vertex in the graph.
     *
     * @param[in] vertex
     *     The input vertex. Must be a valid vertex in the graph.
     *
     * @returns
     *     A view of the vertex's outgoing incident edges and successor vertices.
     */
    [[nodiscard]] constexpr auto
    outgoing_edges(const vertex_type& vertex) const
    {
        WHIRLWIND_ASSERT(contains_vertex(vertex));
        const auto vertex_id = get_vertex_id(vertex);

        const auto r0 = r_[vertex_id];
        const auto r1 = r_[vertex_id + 1];
        auto edges = ranges::views::iota(r0, r1);

        const auto c = c_.data();
        auto heads = std::span(c + r0, c + r1);

        auto to_pair = [](const auto& pair_like) {
            using std::get;
            return std::pair(get<0>(pair_like), get<1>(pair_like));
        };

        return ranges::views::zip(std::move(edges), std::move(heads)) |
               ranges::views::transform(std::move(to_pair));
    }

private:
    container_type<edge_type> r_;
    container_type<vertex_type> c_;
};

WHIRLWIND_NAMESPACE_END
