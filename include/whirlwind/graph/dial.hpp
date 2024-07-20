#pragma once

#include <cmath>
#include <cstddef>
#include <type_traits>
#include <utility>

#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/algorithm/minmax.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

#include <whirlwind/common/assert.hpp>
#include <whirlwind/common/namespace.hpp>
#include <whirlwind/container/queue.hpp>
#include <whirlwind/container/vector.hpp>
#include <whirlwind/math/numbers.hpp>

#include "shortest_path_forest.hpp"

WHIRLWIND_NAMESPACE_BEGIN

template<class Network>
[[nodiscard]] constexpr auto
get_max_admissible_arc_length(const Network& network) -> typename Network::cost_type
{
    using Cost = typename Network::cost_type;
    auto max_arc_length = zero<Cost>();

    for (const auto& tail : network.nodes()) {
        for (const auto& [arc, head] : network.outgoing_arcs(tail)) {
            if (network.is_arc_saturated(arc)) {
                continue;
            }

            const auto arc_length = network.arc_reduced_cost(arc, tail, head);
            WHIRLWIND_ASSERT(!std::isnan(arc_length));
            WHIRLWIND_ASSERT(arc_length >= zero<distance_type>());
            if (std::isinf(arc_length)) {
                continue;
            }

            max_arc_length = std::max(max_arc_length, arc_length);
        }
    }

    return max_arc_length;
}

template<class Distance,
         class Graph,
         template<class> class Container = Vector,
         class Queue = Queue<typename Graph::vertex_type>,
         class ShortestPathForest = ShortestPathForest<Distance, Graph, Container>>
class Dial : public ShortestPathForest {
    WHIRLWIND_STATIC_ASSERT(std::is_integral_v<Distance>);

private:
    using super_type = ShortestPathForest;

public:
    using distance_type = Distance;
    using graph_type = Graph;
    using vertex_type = typename graph_type::vertex_type;
    using edge_type = typename graph_type::edge_type;
    using queue_type = Queue;
    using size_type = std::size_t;

    template<class T>
    using container_type = Container<T>;

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

    constexpr Dial(const graph_type& graph, size_type num_buckets)
        : super_type(graph), buckets_(num_buckets), current_bucket_id_{}
    {
        WHIRLWIND_DEBUG_ASSERT(std::size(buckets_) == num_buckets);
        WHIRLWIND_DEBUG_ASSERT(current_bucket_id() == 0);
    }

    template<class Network>
    explicit constexpr Dial(const Network& network)
        : Dial(network.residual_graph(), [&]() {
              // Get the max finite arc length among admissible arcs in the network.
              const auto max_arc_length = get_max_admissible_arc_length(network);

              // The min number of buckets is the max arc length + 1.
              return static_cast<size_type>(max_arc_length) + 1;
          }())
    {
        WHIRLWIND_STATIC_ASSERT(
                std::is_same_v<typename Network::cost_type, distance_type>);
    }

    [[nodiscard]] constexpr auto
    num_buckets() const noexcept -> size_type
    {
        return std::size(buckets_);
    }

    [[nodiscard]] constexpr auto
    current_bucket_id() const noexcept -> size_type
    {
        return current_bucket_id_;
    }

    [[nodiscard]] constexpr auto
    get_bucket_id(const distance_type& distance) const -> size_type
    {
        WHIRLWIND_DEBUG_ASSERT(distance >= zero<distance_type>());
        return static_cast<size_type>(distance) % num_buckets();
    }

    constexpr void
    push_vertex(vertex_type vertex, const distance_type& distance)
    {
        WHIRLWIND_ASSERT(graph().contains_vertex(vertex));
        WHIRLWIND_ASSERT(distance >= zero<distance_type>());
        WHIRLWIND_ASSERT(num_buckets() >= 1);
        WHIRLWIND_DEBUG_ASSERT(has_reached_vertex(vertex));

        const auto bucket_id = get_bucket_id(distance);
        WHIRLWIND_DEBUG_ASSERT(bucket_id < std::size(buckets_));
        buckets_[bucket_id].push(std::move(vertex));
    }

    constexpr void
    add_source(vertex_type source)
    {
        WHIRLWIND_ASSERT(graph().contains_vertex(source));
        WHIRLWIND_ASSERT(!has_reached_vertex(source));
        WHIRLWIND_ASSERT(std::size(buckets_) > 0);

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
        WHIRLWIND_DEBUG_ASSERT(current_bucket_id() < std::size(buckets_));
        auto& bucket = buckets_[current_bucket_id()];
        WHIRLWIND_ASSERT(!std::empty(bucket));
        auto front = bucket.front();
        WHIRLWIND_DEBUG_ASSERT(has_reached_vertex(front));
        WHIRLWIND_DEBUG_ASSERT(!has_visited_vertex(front));
        bucket.pop();
        return std::pair(std::move(front), distance_to_vertex(front));
    }

    constexpr void
    reach_vertex(edge_type edge,
                 vertex_type tail,
                 vertex_type head,
                 const distance_type& distance)
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
        push_vertex(std::move(head), distance);
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
               const distance_type& distance)
    {
        WHIRLWIND_ASSERT(graph().contains_edge(edge));
        WHIRLWIND_ASSERT(graph().contains_vertex(tail));
        WHIRLWIND_ASSERT(graph().contains_vertex(head));
        WHIRLWIND_ASSERT(distance >= zero<distance_type>());

        WHIRLWIND_DEBUG_ASSERT(has_visited_vertex(tail));
        WHIRLWIND_DEBUG_ASSERT(distance >= distance_to_vertex(tail));

        if (distance < distance_to_vertex(head)) {
            reach_vertex(std::move(edge), std::move(tail), std::move(head), distance);
        }
    }

    [[nodiscard]] constexpr auto
    done() -> bool
    {
        // Handle the unlikely case where the array of buckets is empty.
        const auto n = num_buckets();
        if (n == 0) WHIRLWIND_UNLIKELY {
            return true;
        }

        // Cycle through the ring buffer (updating `current_bucket_id_` along the
        // way) until the first non-empty bucket is found or we arrive back at our
        // initial position.
        const auto old_bucket_id = current_bucket_id_;
        do {
            // If the current bucket is not empty, check each vertex in the bucket
            // until the first unvisited vertex is found or the bucket's contents
            // are exhausted. Visited vertices are removed from the bucket.
            WHIRLWIND_DEBUG_ASSERT(current_bucket_id_ < std::size(buckets_));
            auto& bucket = buckets_[current_bucket_id()];
            while (!std::empty(bucket)) {
                if (!has_visited_vertex(bucket.front())) {
                    return false;
                }
                bucket.pop();
            }

            // Advance to the next bucket in the ring buffer.
            ++current_bucket_id_;
            current_bucket_id_ %= n;

        } while (current_bucket_id_ != old_bucket_id);

        // If we reach this point, all buckets are empty.
        return true;
    }

    constexpr void
    reset()
    {
        super_type::reset();

        // Clear the contents of each bucket and reset the current position to the
        // first bucket.
        ranges::for_each(buckets_, [](auto& bucket) { bucket.clear(); });
        current_bucket_id_ = 0;
    }

private:
    container_type<queue_type> buckets_;
    size_type current_bucket_id_;
};

WHIRLWIND_NAMESPACE_END
