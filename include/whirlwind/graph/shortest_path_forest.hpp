#pragma once

#include <cstdint>
#include <utility>

#include <range/v3/algorithm/fill.hpp>
#include <range/v3/view/filter.hpp>

#include <whirlwind/common/assert.hpp>
#include <whirlwind/common/namespace.hpp>
#include <whirlwind/common/numeric.hpp>
#include <whirlwind/common/vector.hpp>

#include "forest.hpp"

WHIRLWIND_NAMESPACE_BEGIN

template<class Distance,
         class Graph,
         template<class> class Container = Vector,
         class Forest = Forest<Graph, Container>>
class ShortestPathForest : public Forest {
private:
    using super_type = Forest;

protected:
    enum struct label_type : std::uint8_t {
        unreached,
        reached,
        visited,
    };

public:
    using distance_type = Distance;
    using graph_type = Graph;
    using vertex_type = typename graph_type::vertex_type;
    using edge_type = typename graph_type::edge_type;

    template<class T>
    using container_type = Container<T>;

    using super_type::graph;

    explicit constexpr ShortestPathForest(const graph_type& graph)
        : super_type(graph),
          label_(graph.num_vertices(), label_type::unreached),
          distance_(graph.num_vertices(), infinity<distance_type>())
    {
        WHIRLWIND_DEBUG_ASSERT(std::size(label_) == this->graph().num_vertices());
        WHIRLWIND_DEBUG_ASSERT(std::size(distance_) == this->graph().num_vertices());
    }

    [[nodiscard]] constexpr auto
    has_reached_vertex(const vertex_type& vertex) const -> bool
    {
        WHIRLWIND_ASSERT(graph().contains_vertex(vertex));
        const auto vertex_id = graph().get_vertex_id(vertex);
        WHIRLWIND_DEBUG_ASSERT(vertex_id < std::size(label_));
        return label_[vertex_id] != label_type::unreached;
    }

    [[nodiscard]] constexpr auto
    has_visited_vertex(const vertex_type& vertex) const -> bool
    {
        WHIRLWIND_ASSERT(graph().contains_vertex(vertex));
        const auto vertex_id = graph().get_vertex_id(vertex);
        WHIRLWIND_DEBUG_ASSERT(vertex_id < std::size(label_));
        return label_[vertex_id] == label_type::visited;
    }

    /**
     * Mark an unvisited vertex as "reached".
     *
     * Vertices may be "reached" multiple times, but may only be "visited" once.
     * Once a vertex has been marked as "visited", it may no longer be "reached".
     *
     * @param[in] vertex
     *     The input vertex. Must be a valid, unvisited vertex in the graph.
     */
    constexpr void
    label_vertex_reached(const vertex_type& vertex)
    {
        WHIRLWIND_ASSERT(graph().contains_vertex(vertex));
        WHIRLWIND_ASSERT(!has_visited_vertex(vertex));
        const auto vertex_id = graph().get_vertex_id(vertex);
        WHIRLWIND_DEBUG_ASSERT(vertex_id < std::size(label_));
        label_[vertex_id] = label_type::reached;
    }

    /**
     * Mark an unvisited vertex as "visited".
     *
     * Vertices may be "reached" multiple times, but may only be "visited" once.
     * Once a vertex has been marked as "visited", it may no longer be "reached".
     *
     * @param[in] vertex
     *     The input vertex. Must be a valid, unvisited vertex in the graph.
     */
    constexpr void
    label_vertex_visited(const vertex_type& vertex)
    {
        WHIRLWIND_ASSERT(graph().contains_vertex(vertex));
        WHIRLWIND_ASSERT(!has_visited_vertex(vertex));
        const auto vertex_id = graph().get_vertex_id(vertex);
        WHIRLWIND_DEBUG_ASSERT(vertex_id < std::size(label_));
        label_[vertex_id] = label_type::visited;
    }

    [[nodiscard]] constexpr auto
    reached_vertices() const
    {
        return ranges::views::filter(graph().vertices(), [&](const auto& vertex) {
            return has_reached_vertex(vertex);
        });
    }

    [[nodiscard]] constexpr auto
    visited_vertices() const
    {
        return ranges::views::filter(graph().vertices(), [&](const auto& vertex) {
            return has_visited_vertex(vertex);
        });
    }

    [[nodiscard]] constexpr auto
    distance_to_vertex(const vertex_type& vertex) const -> const distance_type&
    {
        WHIRLWIND_ASSERT(graph().contains_vertex(vertex));
        const auto vertex_id = graph().get_vertex_id(vertex);
        WHIRLWIND_DEBUG_ASSERT(vertex_id < std::size(distance_));
        return distance_[vertex_id];
    }

    constexpr void
    set_distance_to_vertex(const vertex_type& vertex, distance_type distance)
    {
        WHIRLWIND_ASSERT(graph().contains_vertex(vertex));
        const auto vertex_id = graph().get_vertex_id(vertex);
        WHIRLWIND_DEBUG_ASSERT(vertex_id < std::size(distance_));
        distance_[vertex_id] = std::move(distance);
    }

    constexpr void
    reset()
    {
        super_type::reset();
        ranges::fill(label_, label_type::unreached);
        ranges::fill(distance_, infinity<distance_type>());
    }

private:
    container_type<label_type> label_;
    container_type<distance_type> distance_;
};

WHIRLWIND_NAMESPACE_END
