#pragma once

#include <array>
#include <generator>
#include <type_traits>
#include <utility>

#include <range/v3/view/cartesian_product.hpp>
#include <range/v3/view/iota.hpp>

#include <whirlwind/common/assert.hpp>
#include <whirlwind/common/compatibility.hpp>
#include <whirlwind/common/namespace.hpp>
#include <whirlwind/common/stddef.hpp>

WHIRLWIND_NAMESPACE_BEGIN

/**
 * A 2-dimensional rectangular grid graph.
 *
 * A graph consisting of an M x N Cartesian grid of vertices. Each vertex has an
 * outgoing edge to each of its four neighboring vertices (except at the boundaries).
 * Vertices are represented by (row,col) index pairs. Edges are represented by unsigned
 * integers.
 *
 * @tparam Dim
 *     The type used to represent row and column indices of vertices in the graph.
 */
template<class Dim = Size>
class RectangularGridGraph {
    WHIRLWIND_STATIC_ASSERT(std::is_integral_v<Dim>);

public:
    using dim_type = Dim;
    using vertex_type = std::pair<dim_type, dim_type>;
    using edge_type = Size;
    using size_type = Size;

    /**
     * Default constructor. Creates an empty `RectangularGridGraph` with no vertices or
     * edges.
     */
    constexpr RectangularGridGraph() = default;

    /**
     * Create a new `RectangularGridGraph`.
     *
     * @param[in] num_rows
     *     The number of rows in the 2-D array of vertices.
     * @param[in] num_cols
     *     The number of columns in the 2-D array of vertices.
     */
    constexpr RectangularGridGraph(dim_type num_rows, dim_type num_cols) noexcept
        : num_rows_(num_rows), num_cols_(num_cols), edge_offsets_{make_edge_offsets()}
    {
        if constexpr (!std::is_unsigned_v<dim_type>) {
            WHIRLWIND_ASSERT(num_rows >= 0);
            WHIRLWIND_ASSERT(num_cols >= 0);
        }
    }

    /** The number of rows of vertices in the graph. */
    [[nodiscard]] constexpr auto
    num_rows() const noexcept -> dim_type
    {
        return num_rows_;
    }

    /** The number of columns of vertices in the graph. */
    [[nodiscard]] constexpr auto
    num_cols() const noexcept -> dim_type
    {
        return num_cols_;
    }

    /** The total number of vertices in the graph. */
    [[nodiscard]] constexpr auto
    num_vertices() const noexcept -> size_type
    {
        return size_type{num_rows()} * size_type{num_cols()};
    }

    /** The total number of edges in the graph. */
    [[nodiscard]] constexpr auto
    num_edges() const noexcept -> size_type
    {
        const auto m = size_type{num_rows()};
        const auto n = size_type{num_cols()};

        if ((m == 0) || (n == 0)) WHIRLWIND_UNLIKELY {
            return 0;
        }

        const auto num_ud_edges = (m - 1) * n;
        const auto num_lr_edges = m * (n - 1);

        return 2 * (num_ud_edges + num_lr_edges);
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
        return size_type{vertex.first} * size_type{num_rows()} +
               size_type{vertex.second};
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
        return size_type{edge};
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
        auto ii = ranges::views::iota(dim_type{0}, num_rows());
        auto jj = ranges::views::iota(dim_type{0}, num_cols());
        return ranges::views::cartesian_product(std::move(ii), std::move(jj));
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
        return (vertex.first < num_rows()) && (vertex.second < num_cols());
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
    outdegree(const vertex_type& vertex) const noexcept -> size_type
    {
        WHIRLWIND_ASSERT(contains_vertex(vertex));

        const auto i = vertex.first;
        const auto j = vertex.second;

        auto n = size_type{4};

        // clang-format off
        if (i == 0) WHIRLWIND_UNLIKELY { --n; }
        if (j == 0) WHIRLWIND_UNLIKELY { --n; }
        if (i == num_rows() - 1) WHIRLWIND_UNLIKELY { --n; }
        if (j == num_cols() - 1) WHIRLWIND_UNLIKELY { --n; }
        // clang-format on

        return n;
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
            -> std::generator<std::pair<edge_type, vertex_type>>
    {
        WHIRLWIND_ASSERT(contains_vertex(vertex));

        const auto i = vertex.first;
        const auto j = vertex.second;

        const auto m = num_rows();
        const auto n = num_cols();

        const auto ei = edge_type{i};
        const auto ej = edge_type{j};
        const auto en = edge_type{n};

        // up
        if (i != 0) WHIRLWIND_LIKELY {
            auto edge = first_up_edge() + (ei - 1) * en + ej;
            auto head = vertex_type(i - 1, j);
            WHIRLWIND_DEBUG_ASSERT(contains_edge(edge));
            WHIRLWIND_DEBUG_ASSERT(contains_vertex(head));
            co_yield std::pair(std::move(edge), std::move(head));
        }

        // left
        if (j != 0) WHIRLWIND_LIKELY {
            auto edge = first_left_edge() + ei * (en - 1) + (ej - 1);
            auto head = vertex_type(i, j - 1);
            WHIRLWIND_DEBUG_ASSERT(contains_edge(edge));
            WHIRLWIND_DEBUG_ASSERT(contains_vertex(head));
            co_yield std::pair(std::move(edge), std::move(head));
        }

        // down
        if (i != m - 1) WHIRLWIND_LIKELY {
            auto edge = first_down_edge() + edge_offsets_[1] + ei * en + ej;
            auto head = vertex_type(i + 1, j);
            WHIRLWIND_DEBUG_ASSERT(contains_edge(edge));
            WHIRLWIND_DEBUG_ASSERT(contains_vertex(head));
            co_yield std::pair(std::move(edge), std::move(head));
        }

        // right
        if (j != n - 1) WHIRLWIND_LIKELY {
            auto edge = first_right_edge() + ei * (en - 1) + ej;
            auto head = vertex_type(i, j + 1);
            WHIRLWIND_DEBUG_ASSERT(contains_edge(edge));
            WHIRLWIND_DEBUG_ASSERT(contains_vertex(head));
            co_yield std::pair(std::move(edge), std::move(head));
        }
    }

protected:
    [[nodiscard]] constexpr auto
    make_edge_offsets() const noexcept
    {
        const auto m = edge_type{num_rows()};
        const auto n = edge_type{num_cols()};

        if ((m == 0) || (n == 0)) WHIRLWIND_UNLIKELY {
            return std::array<edge_type, 3>{0, 0, 0};
        }

        const auto num_ud_edges = (m - 1) * n;
        const auto num_lr_edges = m * (n - 1);

        const auto off0 = num_ud_edges;
        const auto off1 = off0 + num_lr_edges;
        const auto off2 = off1 + num_ud_edges;

        return std::array{off0, off1, off2};
    }

    [[nodiscard]] constexpr auto
    first_up_edge() const noexcept -> edge_type
    {
        return edge_type{0};
    }

    [[nodiscard]] constexpr auto
    first_left_edge() const noexcept -> edge_type
    {
        return edge_offsets_[0];
    }

    [[nodiscard]] constexpr auto
    first_down_edge() const noexcept -> edge_type
    {
        return edge_offsets_[1];
    }

    [[nodiscard]] constexpr auto
    first_right_edge() const noexcept -> edge_type
    {
        return edge_offsets_[2];
    }

private:
    dim_type num_rows_ = {};
    dim_type num_cols_ = {};
    std::array<edge_type, 3> edge_offsets_ = {};
};

WHIRLWIND_NAMESPACE_END
