#pragma once

#include <utility>

#include <range/v3/view/zip.hpp>

#include <whirlwind/common/assert.hpp>
#include <whirlwind/common/namespace.hpp>
#include <whirlwind/common/stddef.hpp>
#include <whirlwind/container/vector.hpp>

WHIRLWIND_NAMESPACE_BEGIN

/**
 * A sequence of (tail,head) vertex pairs.
 *
 * @tparam Vertex
 *     The vertex type.
 * @tparam Container
 *     A `std::vector`-like type template used to store the internal list of vertex
 *     pairs.
 */
template<class Vertex, template<class> class Container = Vector>
class EdgeList {
public:
    using vertex_type = Vertex;
    using value_type = std::pair<vertex_type, vertex_type>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = Size;

    template<class T>
    using container_type = Container<T>;

    /** Default constructor. Creates an empty `EdgeList`. */
    constexpr EdgeList() = default;

    /** Create a new `EdgeList` from a sequence of (tail,head) pairs. */
    constexpr EdgeList(container_type<value_type> edges) : edges_(std::move(edges)) {}

    /**
     * Create a new `EdgeList` from sequences of tail and head vertices.
     *
     * @param[in] tails
     *     A sequence of tail vertices of each edge. Must have the same size as `heads`.
     * @param[in] heads
     *     A sequence of head vertices of each edge. Must have the same size as `tails`.
     */
    template<class InputRange>
    constexpr EdgeList(const InputRange& tails, const InputRange& heads)
        : edges_([&]() {
              WHIRLWIND_ASSERT(std::size(tails) == std::size(heads));
              container_type<value_type> edges;
              for (const auto& [tail, head] : ranges::views::zip(tails, heads)) {
                  edges.emplace_back(tail, head);
              }
              return edges;
          }())
    {}

    [[nodiscard]] constexpr auto
    operator[](size_type pos) -> reference
    {
        WHIRLWIND_ASSERT(pos < size());
        return edges_[pos];
    }

    [[nodiscard]] constexpr auto
    operator[](size_type pos) const -> const_reference
    {
        WHIRLWIND_ASSERT(pos < size());
        return edges_[pos];
    }

    /**
     * Add a new edge.
     *
     * @param[in] tail
     *     The tail vertex of the edge.
     * @param[in] head
     *     The head vertex of the edge.
     *
     * @returns
     *     A reference to the `EdgeList` object.
     */
    constexpr auto
    add_edge(vertex_type tail, vertex_type head) -> EdgeList&
    {
        edges_.emplace_back(std::move(tail), std::move(head));
        return *this;
    }

    /** Erase all edges from the container. */
    constexpr void
    clear()
    {
        edges_.clear();
    }

    /** Get an iterator to the beginning of the container. */
    [[nodiscard]] constexpr auto
    begin() noexcept
    {
        return std::begin(edges_);
    }

    /** Get an iterator to the beginning of the container. */
    [[nodiscard]] constexpr auto
    begin() const noexcept
    {
        return std::begin(edges_);
    }

    /** Get an iterator to the beginning of the container. */
    [[nodiscard]] constexpr auto
    cbegin() const noexcept
    {
        return std::cbegin(edges_);
    }

    /** Get an iterator to the end of the container. */
    [[nodiscard]] constexpr auto
    end() noexcept
    {
        return std::end(edges_);
    }

    /** Get an iterator to the end of the container. */
    [[nodiscard]] constexpr auto
    end() const noexcept
    {
        return std::end(edges_);
    }

    /** Get an iterator to the end of the container. */
    [[nodiscard]] constexpr auto
    cend() const noexcept
    {
        return std::cend(edges_);
    }

    /** Check if the container is empty. */
    [[nodiscard]] constexpr auto
    empty() const noexcept -> bool
    {
        return std::empty(edges_);
    }

    /** The size of the container (the number of edges). */
    [[nodiscard]] constexpr auto
    size() const noexcept -> size_type
    {
        return size_type{std::size(edges_)};
    }

private:
    container_type<value_type> edges_ = {};
};

WHIRLWIND_NAMESPACE_END
