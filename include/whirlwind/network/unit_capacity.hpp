#pragma once

#include <type_traits>
#include <utility>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>

#include <whirlwind/common/assert.hpp>
#include <whirlwind/common/namespace.hpp>
#include <whirlwind/container/vector.hpp>
#include <whirlwind/graph/graph_concepts.hpp>
#include <whirlwind/math/numbers.hpp>

#include "residual_graph.hpp"

WHIRLWIND_NAMESPACE_BEGIN

template<GraphType Graph,
         class Flow,
         template<class> class Container = Vector,
         class ResidualGraphMixin = ResidualGraphMixin<Graph, Container>>
class UnitCapacityMixin : public ResidualGraphMixin {
private:
    using super_type = ResidualGraphMixin;

public:
    using arc_type = typename super_type::arc_type;
    using flow_type = Flow;

    WHIRLWIND_STATIC_ASSERT(std::is_integral_v<flow_type>);

    template<class T>
    using container_type = Container<T>;

    using super_type::arcs;
    using super_type::contains_arc;
    using super_type::get_arc_id;
    using super_type::get_transpose_arc_id;
    using super_type::is_forward_arc;
    using super_type::num_arcs;

    /**
     * Get the upper capacity of an arc in the network.
     *
     * @param[in] arc
     *     The input arc. Must be a valid arc in the network's residual graph (though
     *     its residual capacity may be zero).
     *
     * @returns
     *     The upper capacity of the arc.
     */
    [[nodiscard]] constexpr auto
    arc_capacity([[maybe_unused]] const arc_type& arc) const -> flow_type
    {
        WHIRLWIND_ASSERT(contains_arc(arc));
        return one<flow_type>();
    }

    /**
     * Check whether an arc is saturated.
     *
     * @param[in] arc
     *     The input arc. Must be a valid arc in the network's residual graph (though
     *     its residual capacity may be zero).
     *
     * @returns
     *     True if the arc is saturated (i.e. its residual capacity is zero); otherwise
     *     false.
     */
    [[nodiscard]] constexpr auto
    is_arc_saturated(const arc_type& arc) const -> bool
    {
        WHIRLWIND_ASSERT(contains_arc(arc));
        const auto arc_id = get_arc_id(arc);
        WHIRLWIND_DEBUG_ASSERT(arc_id < std::size(is_arc_saturated_));
        return is_arc_saturated_[arc_id];
    }

    /**
     * Get the residual capacity of an arc.
     *
     * @param[in] arc
     *     The input arc. Must be a valid arc in the network's residual graph (though
     *     its residual capacity may be zero).
     *
     * @returns
     *     The residual capacity of the arc.
     */
    [[nodiscard]] constexpr auto
    arc_residual_capacity(const arc_type& arc) const -> flow_type
    {
        return is_arc_saturated(arc) ? zero<flow_type>() : one<flow_type>();
    }

    /**
     * Get the amount of flow in an arc.
     *
     * @param[in] arc
     *     The input arc. Must be a valid arc in the network's residual graph (though
     *     its residual capacity may be zero).
     *
     * @returns
     *     The amount of flow in the arc.
     */
    [[nodiscard]] constexpr auto
    arc_flow(const arc_type& arc) const -> flow_type
    {
        return is_arc_saturated(arc) ? one<flow_type>() : zero<flow_type>();
    }

    /**
     * Increase flow in an arc.
     *
     * Adds `delta` units of flow to `arc` and removes `delta` units of flow from its
     * corresponding transpose arc in the residual graph. Does not modify the
     * excess/deficit of the arc's head or tail nodes.
     *
     * @param[in] arc
     *     The input arc. Must be a valid, unsaturated arc in the network.
     * @param[in] delta
     *     The amount of additional flow to add to the arc. Must be > 0 and <= the
     *     arc's residual capacity.
     */
    constexpr void
    increase_arc_flow(const arc_type& arc, [[maybe_unused]] const flow_type& delta)
    {
        WHIRLWIND_ASSERT(contains_arc(arc));
        WHIRLWIND_ASSERT(!is_arc_saturated(arc));
        WHIRLWIND_ASSERT(delta == one<flow_type>());
        const auto arc_id = get_arc_id(arc);
        WHIRLWIND_DEBUG_ASSERT(arc_id < std::size(is_arc_saturated_));
        const auto transpose_arc_id = get_transpose_arc_id(arc);
        WHIRLWIND_DEBUG_ASSERT(transpose_arc_id < std::size(is_arc_saturated_));
        is_arc_saturated_[arc_id] = true;
        is_arc_saturated_[transpose_arc_id] = false;
    }

protected:
    template<class... Args>
    constexpr UnitCapacityMixin(Args&&... args)
        : super_type(std::forward<Args>(args)...), is_arc_saturated_([&]() {
              return arcs() | ranges::views::transform([&](const auto& arc) {
                         return !this->is_forward_arc(arc);
                     }) |
                     ranges::to<container_type<bool>>();
          }())
    {
        WHIRLWIND_DEBUG_ASSERT(std::size(is_arc_saturated_) == num_arcs());
    }

private:
    container_type<bool> is_arc_saturated_;
};

WHIRLWIND_NAMESPACE_END
