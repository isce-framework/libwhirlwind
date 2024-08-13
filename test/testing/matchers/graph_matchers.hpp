#pragma once

#include <sstream>
#include <string>
#include <utility>

#include <catch2/matchers/catch_matchers_templated.hpp>

#include <whirlwind/common/namespace.hpp>
#include <whirlwind/graph/graph_concepts.hpp>

WHIRLWIND_NAMESPACE_BEGIN
namespace testing {

template<class Vertex>
class ContainsVertex : public Catch::Matchers::MatcherGenericBase {
public:
    explicit constexpr ContainsVertex(Vertex vertex) noexcept
        : vertex_(std::move(vertex))
    {}

    [[nodiscard]] constexpr auto
    match(const GraphType auto& graph) const -> bool
    {
        return graph.contains_vertex(vertex_);
    }

    [[nodiscard]] auto
    describe() const -> std::string override
    {
        std::stringstream ss;
        ss << "contains vertex " << vertex_;
        return std::move(ss).str();
    }

private:
    Vertex vertex_;
};

template<class Vertex>
ContainsVertex(const Vertex&) -> ContainsVertex<Vertex>;

template<class Vertex>
ContainsVertex(Vertex&&) -> ContainsVertex<Vertex>;

template<class Edge>
class ContainsEdge : public Catch::Matchers::MatcherGenericBase {
public:
    explicit constexpr ContainsEdge(Edge edge) noexcept : edge_(std::move(edge)) {}

    [[nodiscard]] constexpr auto
    match(const GraphType auto& graph) const -> bool
    {
        return graph.contains_edge(edge_);
    }

    [[nodiscard]] auto
    describe() const -> std::string override
    {
        std::stringstream ss;
        ss << "contains edge " << edge_;
        return std::move(ss).str();
    }

private:
    Edge edge_;
};

template<class Edge>
ContainsEdge(const Edge&) -> ContainsEdge<Edge>;

template<class Edge>
ContainsEdge(Edge&&) -> ContainsEdge<Edge>;

} // namespace testing
WHIRLWIND_NAMESPACE_END
