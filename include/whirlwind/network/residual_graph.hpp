#pragma once

#include <utility>

#include <range/v3/view/filter.hpp>

#include <whirlwind/common/assert.hpp>
#include <whirlwind/common/namespace.hpp>
#include <whirlwind/common/numeric.hpp>
#include <whirlwind/common/stddef.hpp>
#include <whirlwind/container/vector.hpp>
#include <whirlwind/graph/rectangular_grid_graph.hpp>

#include "residual_graph_traits.hpp"

WHIRLWIND_NAMESPACE_BEGIN

namespace detail {

template<class Graph>
class BasicResidualGraphMixin {
public:
    using graph_type = Graph;
    using residual_graph_type = ResidualGraphTraits<graph_type>::type;
    using node_type = residual_graph_type::vertex_type;
    using arc_type = residual_graph_type::edge_type;
    using size_type = Size;

    /** The residual graph of the network. */
    [[nodiscard]] constexpr auto
    residual_graph() const noexcept -> const residual_graph_type&
    {
        return residual_graph_;
    }

    /** The total number of nodes in the network. */
    [[nodiscard]] constexpr auto
    num_nodes() const noexcept -> size_type
    {
        return residual_graph().num_vertices();
    }

    /**
     * The total number of arcs in the network's residual graph (including arcs whose
     * residual capacity is zero).
     */
    [[nodiscard]] constexpr auto
    num_arcs() const noexcept -> size_type
    {
        return residual_graph().num_edges();
    }

    [[nodiscard]] constexpr auto
    num_forward_arcs() const noexcept -> size_type
    {
        WHIRLWIND_DEBUG_ASSERT(is_even(num_arcs()));
        return num_arcs() / 2;
    }

    /** Check whether the network contains the specified node. */
    [[nodiscard]] constexpr auto
    contains_node(const node_type& node) const -> bool
    {
        return residual_graph().contains_vertex(node);
    }

    /**
     * Check whether the network's residual graph contains the specified arc.
     *
     * Arcs with zero residual capacity are considered to be contained in the residual
     * graph.
     */
    [[nodiscard]] constexpr auto
    contains_arc(const arc_type& arc) const -> bool
    {
        return residual_graph().contains_edge(arc);
    }

    /**
     * Get the unique array index of a node.
     *
     * Given a node in the network, get the associated node index in the range [0, N),
     * where N is the total number of nodes.
     *
     * @param[in] node
     *     The input node. Must be a valid node in the network.
     *
     * @returns
     *     The node index.
     */
    [[nodiscard]] constexpr auto
    get_node_id(const node_type& node) const -> size_type
    {
        WHIRLWIND_ASSERT(contains_node(node));
        return residual_graph().get_vertex_id(node);
    }

    /**
     * Get the unique array index of an arc.
     *
     * Given an arc in the network's residual graph, get the associated arc index in the
     * range [0, A), where A is the total number of arcs.
     *
     * @param[in] arc
     *     The input arc. Must be a valid arc in the network's residual graph (though
     *     its residual capacity may be zero).
     *
     * @returns
     *     The arc index.
     */
    [[nodiscard]] constexpr auto
    get_arc_id(const arc_type& arc) const -> size_type
    {
        WHIRLWIND_ASSERT(contains_arc(arc));
        return residual_graph().get_edge_id(arc);
    }

    /**
     * Iterate over nodes in the network.
     *
     * Returns a view of all nodes in the network in order from smallest node index to
     * largest.
     */
    [[nodiscard]] constexpr auto
    nodes() const
    {
        return residual_graph().vertices();
    }

    /**
     * Iterate over arcs in the network's residual graph.
     *
     * Returns a view of all arcs in the residual graph (including arcs whose residual
     * capacity is zero) in order from smallest arc index to largest.
     */
    [[nodiscard]] constexpr auto
    arcs() const
    {
        return residual_graph().edges();
    }

    /**
     * Iterate over outgoing arcs (and corresponding head nodes) of a node.
     *
     * Returns a view of ordered (arc,head) pairs over all arcs emanating from the
     * specified node in the network's residual graph.
     *
     * @param[in] node
     *     The input node. Must be a valid node in the network.
     *
     * @returns
     *     A view of the node's outgoing incident arcs and successor nodes in the
     *     residual graph.
     */
    [[nodiscard]] constexpr auto
    outgoing_arcs(const node_type& node) const
    {
        WHIRLWIND_ASSERT(contains_node(node));
        return residual_graph().outgoing_edges(node);
    }

protected:
    constexpr BasicResidualGraphMixin(residual_graph_type residual_graph)
        : residual_graph_(std::move(residual_graph))
    {}

private:
    residual_graph_type residual_graph_;
};

} // namespace detail

template<class Graph, template<class> class Container = Vector>
class ResidualGraphMixin : public detail::BasicResidualGraphMixin<Graph> {
private:
    using super_type = detail::BasicResidualGraphMixin<Graph>;

public:
    using arc_type = super_type::arc_type;
    using size_type = super_type::size_type;
    using residual_graph_type = super_type::residual_graph_type;

    template<class T>
    using container_type = Container<T>;

    using super_type::arcs;
    using super_type::contains_arc;
    using super_type::get_arc_id;
    using super_type::num_arcs;

    /**
     * Check whether an arc in the network's residual graph is a forward arc.
     *
     * Forward arcs are those found in the original graph, whereas reverse arcs are
     * found only in the residual graph.
     *
     * @param[in] arc
     *     The input arc. Must be a valid arc in the network's residual graph (though
     *     its residual capacity may be zero).
     *
     * @returns
     *     True if `arc` is a forward arc; otherwise false.
     */
    [[nodiscard]] constexpr auto
    is_forward_arc(const arc_type& arc) const -> bool
    {
        WHIRLWIND_ASSERT(contains_arc(arc));
        const auto arc_id = get_arc_id(arc);
        WHIRLWIND_DEBUG_ASSERT(arc_id < std::size(is_forward_arc_));
        return is_forward_arc_[arc_id];
    }

    [[nodiscard]] constexpr auto
    forward_arcs() const
    {
        return ranges::views::filter(
                arcs(), [&](const auto& arc) { return is_forward_arc(arc); });
    }

    /**
     * Given the edge index of an edge in the original graph, get the arc index of the
     * corresponding arc in the residual graph.
     *
     * @param[in] edge_id
     *     The input edge index. Must be the index of a valid edge in the original
     *     graph.
     *
     * @returns
     *     The arc index of the corresponding arc in the residual graph.
     */
    [[nodiscard]] constexpr auto
    get_residual_graph_arc_id(size_type edge_id) const -> size_type
    {
        WHIRLWIND_ASSERT(edge_id < std::size(residual_graph_arc_id_));
        return residual_graph_arc_id_[edge_id];
    }

    /**
     * Given a forward or reverse arc in the network's residual graph, get the index of
     * its transpose arc.
     *
     * If the input arc was a forward arc from `tail` to `head`, its transpose arc is a
     * reverse arc from `head` to `tail`. Likewise, if the input arc was a reverse arc
     * from `head` to `tail`, its transpose arc is a forward arc from `tail` to `head`.
     *
     * @param[in] arc
     *     The input arc. Must be a valid arc in the network's residual graph (though
     *     its residual capacity may be zero).
     *
     * @returns
     *     The arc index of the transpose arc.
     */
    [[nodiscard]] constexpr auto
    get_transpose_arc_id(const arc_type& arc) const -> size_type
    {
        WHIRLWIND_ASSERT(contains_arc(arc));
        const auto arc_id = get_arc_id(arc);
        WHIRLWIND_DEBUG_ASSERT(arc_id < std::size(transpose_arc_id_));
        return transpose_arc_id_[arc_id];
    }

protected:
    constexpr ResidualGraphMixin(residual_graph_type residual_graph,
                                 container_type<bool> is_forward_arc,
                                 container_type<size_type> residual_graph_arc_id,
                                 container_type<size_type> transpose_arc_id)
        : super_type(std::move(residual_graph)),
          is_forward_arc_(std::move(is_forward_arc)),
          residual_graph_arc_id_(std::move(residual_graph_arc_id)),
          transpose_arc_id_(std::move(transpose_arc_id))
    {
        WHIRLWIND_ASSERT(2 * std::size(residual_graph_arc_id_) ==
                         std::size(transpose_arc_id_));
        WHIRLWIND_ASSERT(std::size(is_forward_arc_) == residual_graph().num_edges());
        WHIRLWIND_ASSERT(std::size(transpose_arc_id_) == residual_graph().num_edges());
    }

private:
    container_type<bool> is_forward_arc_;
    container_type<size_type> residual_graph_arc_id_;
    container_type<size_type> transpose_arc_id_;
};

// Partial specialization for `RectangularGridGraph`.
template<class Dim, template<class> class Container>
class ResidualGraphMixin<RectangularGridGraph<1, Dim>, Container>
    : public detail::BasicResidualGraphMixin<RectangularGridGraph<1, Dim>> {
private:
    using super_type = detail::BasicResidualGraphMixin<RectangularGridGraph<1, Dim>>;

public:
    using graph_type = super_type::graph_type;
    using residual_graph_type = super_type::residual_graph_type;
    using arc_type = super_type::arc_type;
    using size_type = super_type::size_type;

    template<class T>
    using container_type = Container<T>;

    using super_type::arcs;
    using super_type::contains_arc;
    using super_type::get_arc_id;
    using super_type::num_forward_arcs;

    [[nodiscard]] constexpr auto
    is_forward_arc(const arc_type& arc) const -> bool
    {
        WHIRLWIND_ASSERT(contains_arc(arc));
        const auto arc_id = get_arc_id(arc);
        return is_even(arc_id);
    }

    [[nodiscard]] constexpr auto
    forward_arcs() const
    {
        return ranges::views::filter(
                arcs(), [&](const auto& arc) { return is_forward_arc(arc); });
    }

    [[nodiscard]] constexpr auto
    get_residual_graph_arc_id(size_type edge_id) const -> size_type
    {
        return 2 * edge_id;
    }

    [[nodiscard]] constexpr auto
    get_edge_id(const arc_type& forward_arc) const -> size_type
    {
        WHIRLWIND_ASSERT(contains_arc(forward_arc));
        WHIRLWIND_ASSERT(is_forward_arc(forward_arc));
        auto arc_id = get_arc_id(forward_arc);
        WHIRLWIND_DEBUG_ASSERT(is_even(arc_id));
        return arc_id / 2;
    }

    [[nodiscard]] constexpr auto
    get_transpose_arc_id(const arc_type& arc) const -> size_type
    {
        WHIRLWIND_ASSERT(contains_arc(arc));
        auto arc_id = get_arc_id(arc);

        const auto n = num_forward_arcs();
        WHIRLWIND_DEBUG_ASSERT(n >= 1);

        if (is_forward_arc(arc)) {
            if (arc_id < n) {
                return arc_id + n + 1;
            } else {
                return arc_id - n + 1;
            }
        } else {
            if (arc_id < n) {
                return arc_id + n - 1;
            } else {
                return arc_id - n - 1;
            }
        }
    }

protected:
    constexpr ResidualGraphMixin(const graph_type& original_graph)
        : super_type(residual_graph_type(original_graph.num_rows(),
                                         original_graph.num_cols()))
    {}
};

WHIRLWIND_NAMESPACE_END
