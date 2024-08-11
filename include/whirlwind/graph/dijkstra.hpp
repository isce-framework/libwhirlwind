#pragma once

#include <utility>

#include <whirlwind/common/assert.hpp>
#include <whirlwind/common/namespace.hpp>
#include <whirlwind/container/heap.hpp>
#include <whirlwind/container/vector.hpp>
#include <whirlwind/math/numbers.hpp>

#include "forest_concepts.hpp"
#include "graph_concepts.hpp"
#include "shortest_path_forest.hpp"

WHIRLWIND_NAMESPACE_BEGIN

template<class Distance,
         GraphType Graph,
         template<class> class Container = Vector,
         class Heap = BinaryHeap<typename Graph::vertex_type, Distance, Container>,
         MutableShortestPathForestType ShortestPaths =
                 ShortestPathForest<Distance, Graph, Container>>
class Dijkstra : public ShortestPaths {
private:
    using base_type = ShortestPaths;

public:
    using distance_type = Distance;
    using graph_type = Graph;
    using vertex_type = typename graph_type::vertex_type;
    using edge_type = typename graph_type::edge_type;
    using heap_type = Heap;

    using base_type::distance_to_vertex;
    using base_type::graph;
    using base_type::has_reached_vertex;
    using base_type::has_visited_vertex;
    using base_type::is_root_vertex;
    using base_type::label_vertex_reached;
    using base_type::label_vertex_visited;
    using base_type::make_root_vertex;
    using base_type::predecessor_vertex;
    using base_type::set_distance_to_vertex;
    using base_type::set_predecessor;

    explicit constexpr Dijkstra(const graph_type& g) : base_type(g), heap_{}
    {
        WHIRLWIND_DEBUG_ASSERT(std::empty(heap_));
    }

    template<class Network>
    explicit constexpr Dijkstra(const Network& network)
        : Dijkstra(network.residual_graph())
    {
        WHIRLWIND_STATIC_ASSERT(std::is_same_v<typename Network::cost_type, Distance>);
    }

    [[nodiscard]] constexpr auto
    heap() const noexcept -> const heap_type&
    {
        return heap_;
    }

    [[nodiscard]] constexpr auto
    heap() noexcept -> heap_type&
    {
        return heap_;
    }

    constexpr void
    push_vertex(vertex_type vertex, distance_type distance)
    {
        WHIRLWIND_ASSERT(graph().contains_vertex(vertex));
        WHIRLWIND_ASSERT(distance >= zero<distance_type>());
        WHIRLWIND_DEBUG_ASSERT(has_reached_vertex(vertex));
        heap().emplace(std::move(vertex), std::move(distance));
    }

    constexpr void
    add_source(vertex_type source)
    {
        WHIRLWIND_ASSERT(graph().contains_vertex(source));
        WHIRLWIND_ASSERT(!has_reached_vertex(source));

        make_root_vertex(source);
        WHIRLWIND_DEBUG_ASSERT(predecessor_vertex(source) == source);

        label_vertex_reached(source);
        set_distance_to_vertex(source, zero<distance_type>());
        push_vertex(std::move(source), zero<distance_type>());
    }

    constexpr auto
    pop_next_unvisited_vertex()
    {
        WHIRLWIND_ASSERT(!std::empty(heap()));
        auto top = heap().top();
        using std::get;
        WHIRLWIND_DEBUG_ASSERT(has_reached_vertex(get<0>(top)));
        WHIRLWIND_DEBUG_ASSERT(!has_visited_vertex(get<0>(top)));
        heap().pop();
        return top;
    }

    constexpr void
    reach_vertex(edge_type edge,
                 vertex_type tail,
                 vertex_type head,
                 distance_type distance)
    {
        WHIRLWIND_ASSERT(graph().contains_edge(edge));
        WHIRLWIND_ASSERT(graph().contains_vertex(tail));
        WHIRLWIND_ASSERT(graph().contains_vertex(head));
        WHIRLWIND_ASSERT(distance >= zero<distance_type>());

        WHIRLWIND_DEBUG_ASSERT(has_visited_vertex(tail));
        WHIRLWIND_DEBUG_ASSERT(!has_visited_vertex(head));
        WHIRLWIND_DEBUG_ASSERT(distance >= distance_to_vertex(tail));

        set_predecessor(head, std::move(tail), std::move(edge));
        WHIRLWIND_DEBUG_ASSERT(!is_root_vertex(head));
        label_vertex_reached(head);
        set_distance_to_vertex(head, distance);
        push_vertex(std::move(head), std::move(distance));
    }

    constexpr void
    visit_vertex(const vertex_type& vertex, [[maybe_unused]] distance_type distance)
    {
        WHIRLWIND_ASSERT(graph().contains_vertex(vertex));
        WHIRLWIND_ASSERT(distance >= zero<distance_type>());
        WHIRLWIND_DEBUG_ASSERT(has_reached_vertex(vertex));
        label_vertex_visited(vertex);
    }

    constexpr void
    relax_edge(edge_type edge,
               vertex_type tail,
               vertex_type head,
               distance_type distance)
    {
        WHIRLWIND_ASSERT(graph().contains_edge(edge));
        WHIRLWIND_ASSERT(graph().contains_vertex(tail));
        WHIRLWIND_ASSERT(graph().contains_vertex(head));
        WHIRLWIND_ASSERT(distance >= zero<distance_type>());

        WHIRLWIND_DEBUG_ASSERT(has_visited_vertex(tail));
        WHIRLWIND_DEBUG_ASSERT(distance >= distance_to_vertex(tail));

        if (distance < distance_to_vertex(head)) {
            reach_vertex(std::move(edge), std::move(tail), std::move(head),
                         std::move(distance));
        }
    }

    [[nodiscard]] constexpr auto
    done() -> bool
    {
        // TODO: Some heap implementations support modifying the priority of nodes in
        // the heap. In this case, each vertex should be inserted into the heap only
        // once, and `done()` could simply check whether the heap is empty.
        while (!heap().empty()) {
            using std::get;
            const auto& vertex = get<0>(heap().top());
            if (!has_visited_vertex(vertex)) {
                return false;
            }
            heap().pop();
        }
        return true;
    }

    constexpr void
    reset()
    {
        base_type::reset();
        heap().clear();
        WHIRLWIND_DEBUG_ASSERT(std::empty(heap()));
    }

private:
    heap_type heap_;
};

WHIRLWIND_NAMESPACE_END
