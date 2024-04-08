#pragma once

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include <range/v3/algorithm/fill.hpp>
#include <range/v3/view/filter.hpp>

#include <whirlwind/common/assert.hpp>
#include <whirlwind/common/heap.hpp>
#include <whirlwind/common/namespace.hpp>
#include <whirlwind/common/numeric.hpp>

#include "forest.hpp"
#include "graph_traits.hpp"

WHIRLWIND_NAMESPACE_BEGIN

template<class Distance,
         class Graph,
         template<class> class Container = std::vector,
         class Heap = BinaryHeap<typename Graph::vertex_type, Distance, Container>,
         class Forest = Forest<Graph, Container>>
class Dijkstra : public Forest {
protected:
    enum struct label_type : std::uint8_t {
        unreached,
        reached,
        visited,
    };

public:
    using distance_type = Distance;
    using graph_type = Graph;
    using vertex_type = GraphTraits<graph_type>::vertex_type;
    using edge_type = GraphTraits<graph_type>::edge_type;
    using heap_type = Heap;
    using value_type = typename heap_type::value_type;
    using forest_type = Forest;

    template<class T>
    using container_type = Container<T>;

    using forest_type::graph;
    using forest_type::make_root_vertex;
    using forest_type::set_predecessor;

    explicit constexpr Dijkstra(const graph_type& graph)
        : forest_type(graph),
          label_(graph.num_vertices(), label_type::unreached),
          distance_(graph.num_vertices(), infinity<distance_type>()),
          heap_{}
    {
        WHIRLWIND_DEBUG_ASSERT(std::size(label_) == graph().num_vertices());
        WHIRLWIND_DEBUG_ASSERT(std::size(distance_) == graph().num_vertices());
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
    add_source(const vertex_type& source)
    {
        WHIRLWIND_ASSERT(graph().contains_vertex(source));
        WHIRLWIND_ASSERT(!has_reached_vertex(source));
        make_root_vertex(source);
        label_vertex_reached(source);
        set_distance_to_vertex(source, zero<distance_type>());
        heap_.emplace(source, zero<distance_type>());
    }

    constexpr auto
    pop_next_unvisited_vertex() -> value_type
    {
        WHIRLWIND_ASSERT(!heap_.empty());
        auto top = heap_.top();
        using std::get;
        WHIRLWIND_DEBUG_ASSERT(has_reached_vertex(get<0>(top)));
        WHIRLWIND_DEBUG_ASSERT(!has_visited_vertex(get<0>(top)));
        heap_.pop();
        return top;
    }

    constexpr void
    visit_vertex(const vertex_type& vertex, distance_type distance)
    {
        WHIRLWIND_ASSERT(graph().contains_vertex(vertex));
        WHIRLWIND_ASSERT(distance >= zero<distance_type>());
        WHIRLWIND_DEBUG_ASSERT(has_reached_vertex(source));
        label_vertex_visited(vertex);
        set_distance_to_vertex(vertex, std::move(distance));
    }

    constexpr void
    relax_edge(const edge_type& edge,
               const vertex_type& tail,
               const vertex_type& head,
               distance_type distance)
    {
        WHIRLWIND_ASSERT(graph().contains_edge(edge));
        WHIRLWIND_ASSERT(graph().contains_vertex(tail));
        WHIRLWIND_ASSERT(graph().contains_vertex(head));
        WHIRLWIND_ASSERT(has_visited_vertex(tail));
        WHIRLWIND_ASSERT(distance >= zero<distance_type>());

        if (has_visited_vertex(head)) {
            return;
        }

        WHIRLWIND_DEBUG_ASSERT(distance <= distance_to_vertex(head));
        set_predecessor(head, tail, edge);
        WHIRLWIND_DEBUG_ASSERT(!is_vertex_root(head));
        label_vertex_reached(head);
        heap_.emplace(head, std::move(distance));
    }

    [[nodiscard]] constexpr auto
    done() -> bool
    {
        // TODO: Some heap implementations support modifying the priority of nodes in
        // the heap. In this case, each vertex should be inserted into the heap only
        // once, and `done()` could simply check whether the heap is empty.
        while (!heap_.empty()) {
            using std::get;
            const auto& vertex = get<0>(heap_.top());
            if (!has_visited_vertex(vertex)) {
                return false;
            }
            heap_.pop();
        }
        return true;
    }

    constexpr void
    reset()
    {
        forest_type::reset();
        ranges::fill(label_, label_type::unreached);
        ranges::fill(distance_, infinity<distance_type>());
        heap_.clear();
        WHIRLWIND_DEBUG_ASSERT(heap_.empty());
    }

private:
    container_type<label_type> label_;
    container_type<distance_type> distance_;
    heap_type heap_;
};

WHIRLWIND_NAMESPACE_END
