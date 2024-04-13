#pragma once

#include <utility>

#include <range/v3/view/facade.hpp>

#include <whirlwind/common/assert.hpp>
#include <whirlwind/common/namespace.hpp>

WHIRLWIND_NAMESPACE_BEGIN

template<class Forest>
class PredecessorsView : public ranges::view_facade<PredecessorsView<Forest>> {
    friend ranges::range_access;

public:
    using forest_type = Forest;
    using vertex_type = typename Forest::vertex_type;
    using value_type = typename Forest::pred_type;

    constexpr PredecessorsView(const forest_type* forest, vertex_type vertex)
        : forest_(forest), vertex_(std::move(vertex))
    {
        WHIRLWIND_ASSERT(forest_ != nullptr);
        WHIRLWIND_ASSERT(this->forest().graph().contains_vertex(vertex_));
    }

    [[nodiscard]] constexpr auto
    forest() const noexcept -> const forest_type&
    {
        WHIRLWIND_DEBUG_ASSERT(forest_ != nullptr);
        return *forest_;
    }

    [[nodiscard]] constexpr auto
    current_vertex() const noexcept -> const vertex_type&
    {
        WHIRLWIND_DEBUG_ASSERT(forest().graph().contains_vertex(vertex_));
        return vertex_;
    }

    constexpr void
    set_current_vertex(vertex_type vertex)
    {
        WHIRLWIND_DEBUG_ASSERT(forest().graph().contains_vertex(vertex));
        vertex_ = std::move(vertex);
    }

protected:
    constexpr PredecessorsView() = default;

    [[nodiscard]] constexpr auto
    read() const -> value_type
    {
        WHIRLWIND_DEBUG_ASSERT(!forest().is_root_vertex(current_vertex()));
        return forest().predecessor(current_vertex());
    }

    [[nodiscard]] constexpr auto
    equal(ranges::default_sentinel_t) const -> bool
    {
        return forest().is_root_vertex(current_vertex());
    }

    constexpr void
    next()
    {
        WHIRLWIND_DEBUG_ASSERT(!forest().is_root_vertex(current_vertex()));
        auto pred_vertex = forest().predecessor_vertex(current_vertex());
        set_current_vertex(std::move(pred_vertex));
    }

private:
    const forest_type* forest_;
    vertex_type vertex_;
};

WHIRLWIND_NAMESPACE_END
