#pragma once

#include <utility>

#include <whirlwind/common/assert.hpp>
#include <whirlwind/common/namespace.hpp>
#include <whirlwind/container/vector.hpp>
#include <whirlwind/math/numbers.hpp>

#include "residual_graph.hpp"

WHIRLWIND_NAMESPACE_BEGIN

template<class Graph,
         class Flow,
         template<class> class Container = Vector,
         class ResidualGraphMixin = ResidualGraphMixin<Graph, Container>>
class UncapacitatedMixin : public ResidualGraphMixin {
private:
    using super_type = ResidualGraphMixin;

public:
    using arc_type = typename super_type::arc_type;
    using flow_type = Flow;

    template<class T>
    using container_type = Container<T>;

    using super_type::contains_arc;
    using super_type::get_arc_id;
    using super_type::get_transpose_arc_id;
    using super_type::is_forward_arc;
    using super_type::num_forward_arcs;

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
        return infinity<flow_type>();
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
        WHIRLWIND_ASSERT(contains_arc(arc));

        if (!is_forward_arc(arc)) {
            return infinity<flow_type>();
        }

        const auto arc_id = get_arc_id(arc);
        WHIRLWIND_DEBUG_ASSERT(arc_id < std::size(arc_flow_));
        return arc_flow_[arc_id];
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
        WHIRLWIND_ASSERT(contains_arc(arc));

        if (is_forward_arc(arc)) {
            return infinity<flow_type>();
        }

        const auto transpose_arc_id = get_transpose_arc_id(arc);
        WHIRLWIND_DEBUG_ASSERT(transpose_arc_id < std::size(arc_flow_));
        return arc_flow_[transpose_arc_id];
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
        if (is_forward_arc(arc)) {
            return false;
        }
        return arc_residual_capacity(arc) > zero<flow_type>();
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
    increase_arc_flow(const arc_type& arc, const flow_type& delta)
    {
        WHIRLWIND_ASSERT(contains_arc(arc));
        WHIRLWIND_ASSERT(arc_residual_capacity(arc) >= delta);

        if (is_forward_arc(arc)) {
            const auto arc_id = get_arc_id(arc);
            WHIRLWIND_DEBUG_ASSERT(arc_id < std::size(arc_flow_));
            arc_flow_[arc_id] += delta;
        } else {
            const auto transpose_arc_id = get_transpose_arc_id(arc);
            WHIRLWIND_DEBUG_ASSERT(transpose_arc_id < std::size(arc_flow_));
            arc_flow_[transpose_arc_id] -= delta;
        }
    }

protected:
    template<class... Args>
    constexpr UncapacitatedMixin(Args&&... args)
        : super_type(std::forward<Args>(args)...),
          arc_flow_(num_forward_arcs(), zero<flow_type>())
    {
        WHIRLWIND_DEBUG_ASSERT(std::size(arc_flow_) == num_forward_arcs());
    }

private:
    container_type<flow_type> arc_flow_;
};

WHIRLWIND_NAMESPACE_END
