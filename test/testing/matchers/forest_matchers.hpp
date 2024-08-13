#pragma once

#include <string>

#include <catch2/matchers/catch_matchers_templated.hpp>

#include <whirlwind/common/namespace.hpp>
#include <whirlwind/graph/forest_concepts.hpp>

WHIRLWIND_NAMESPACE_BEGIN
namespace testing {

template<ForestType Forest>
class IsRootVertexIn : public Catch::Matchers::MatcherGenericBase {
public:
    explicit constexpr IsRootVertexIn(const Forest& forest) noexcept : forest_(forest)
    {}

    [[nodiscard]] constexpr auto
    match(const typename Forest::vertex_type& vertex) const -> bool
    {
        return forest_.is_root_vertex(vertex);
    }

    [[nodiscard]] auto
    describe() const -> std::string override
    {
        return "is a root vertex";
    }

private:
    const Forest& forest_; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
};

template<ShortestPathForestType Forest>
class WasReachedBy : public Catch::Matchers::MatcherGenericBase {
public:
    explicit constexpr WasReachedBy(const Forest& forest) noexcept : forest_(forest) {}

    [[nodiscard]] constexpr auto
    match(const typename Forest::vertex_type& vertex) const -> bool
    {
        return forest_.has_reached_vertex(vertex);
    }

    [[nodiscard]] auto
    describe() const -> std::string override
    {
        return "vertex was reached";
    }

private:
    const Forest& forest_; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
};

template<ShortestPathForestType Forest>
class WasVisitedBy : public Catch::Matchers::MatcherGenericBase {
public:
    explicit constexpr WasVisitedBy(const Forest& forest) noexcept : forest_(forest) {}

    [[nodiscard]] constexpr auto
    match(const typename Forest::vertex_type& vertex) const -> bool
    {
        return forest_.has_visited_vertex(vertex);
    }

    [[nodiscard]] auto
    describe() const -> std::string override
    {
        return "vertex was visited";
    }

private:
    const Forest& forest_; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
};

} // namespace testing
WHIRLWIND_NAMESPACE_END
