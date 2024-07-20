#pragma once

#include <utility>

#include <whirlwind/common/assert.hpp>
#include <whirlwind/common/namespace.hpp>
#include <whirlwind/common/numeric.hpp>
#include <whirlwind/container/heap.hpp>
#include <whirlwind/container/vector.hpp>

#include "shortest_path_forest.hpp"

WHIRLWIND_NAMESPACE_BEGIN

template<class Distance,
         class Graph,
         template<class> class Container = Vector,
         class Heap = BinaryHeap<typename Graph::vertex_type, Distance, Container>,
         class ShortestPathForest = ShortestPathForest<Distance, Graph, Container>>
class Dijkstra : public ShortestPathForest {
private:
    using super_type = ShortestPathForest;

public:
    using distance_type = Distance;
    using graph_type = Graph;
    using vertex_type = typename graph_type::vertex_type;
    using edge_type = typename graph_type::edge_type;
    using heap_type = Heap;

    using super_type::depth;
    using super_type::distance_to_vertex;
    using super_type::graph;
    using super_type::has_reached_vertex;
    using super_type::has_visited_vertex;
    using super_type::is_root_vertex;
    using super_type::label_vertex_reached;
    using super_type::label_vertex_visited;
    using super_type::make_root_vertex;
    using super_type::predecessor_vertex;
    using super_type::set_distance_to_vertex;
    using super_type::set_predecessor;

    explicit constexpr Dijkstra(const graph_type& graph) : super_type(graph), heap_{}
    {
        WHIRLWIND_DEBUG_ASSERT(std::empty(heap_));
    }

    template<class Network>
    explicit constexpr Dijkstra(const Network& network)
        : Dijkstra(network.residual_graph())
    {
        WHIRLWIND_STATIC_ASSERT(std::is_same_v<typename Network::cost_type, Distance>);
    }

    constexpr void
    push_vertex(vertex_type vertex, distance_type distance)
    {
        WHIRLWIND_ASSERT(graph().contains_vertex(vertex));
        WHIRLWIND_ASSERT(distance >= zero<distance_type>());
        WHIRLWIND_DEBUG_ASSERT(has_reached_vertex(vertex));
        heap_.emplace(std::move(vertex), std::move(distance));
    }

    constexpr void
    add_source(vertex_type source)
    {
        WHIRLWIND_ASSERT(graph().contains_vertex(source));
        WHIRLWIND_ASSERT(!has_reached_vertex(source));

        make_root_vertex(source);
        WHIRLWIND_DEBUG_ASSERT(depth(source) == 0);
        WHIRLWIND_DEBUG_ASSERT(predecessor_vertex(source) == source);

        label_vertex_reached(source);
        set_distance_to_vertex(source, zero<distance_type>());
        push_vertex(std::move(source), zero<distance_type>());
    }

    constexpr auto
    pop_next_unvisited_vertex()
    {
        WHIRLWIND_ASSERT(!std::empty(heap_));
        auto top = heap_.top();
        using std::get;
        WHIRLWIND_DEBUG_ASSERT(has_reached_vertex(get<0>(top)));
        WHIRLWIND_DEBUG_ASSERT(!has_visited_vertex(get<0>(top)));
        heap_.pop();
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
        super_type::reset();
        heap_.clear();
        WHIRLWIND_DEBUG_ASSERT(std::empty(heap_));
    }

private:
    heap_type heap_;
};

WHIRLWIND_NAMESPACE_END
