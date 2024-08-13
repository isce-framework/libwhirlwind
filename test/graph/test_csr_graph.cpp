#include <cstddef>
#include <type_traits>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include <whirlwind/graph/csr_graph.hpp>
#include <whirlwind/graph/edge_list.hpp>

#include "../testing/matchers/graph_matchers.hpp"
#include "../testing/string_conversions.hpp" // IWYU pragma: keep

namespace {

namespace CM = Catch::Matchers;
namespace ww = whirlwind;

CATCH_TEST_CASE("CSRGraph (empty)", "[graph]")
{
    const auto graph = ww::CSRGraph();

    CATCH_SECTION("num_{vertices,edges}")
    {
        CATCH_CHECK(graph.num_vertices() == 0U);
        CATCH_CHECK(graph.num_edges() == 0U);
    }

    CATCH_SECTION("contains_{vertex,edge}")
    {
        CATCH_CHECK_THAT(graph, !ww::testing::ContainsVertex(0U));
        CATCH_CHECK_THAT(graph, !ww::testing::ContainsEdge(0U));
    }
}

CATCH_TEST_CASE("CSRGraph", "[graph]")
{
    auto edgelist = ww::EdgeList();
    edgelist.add_edge(0U, 1U);
    edgelist.add_edge(0U, 2U);
    edgelist.add_edge(0U, 3U);
    edgelist.add_edge(2U, 1U);
    edgelist.add_edge(3U, 0U);

    const auto graph = ww::CSRGraph(edgelist);

    using Vertex = decltype(graph)::vertex_type;
    using Edge = decltype(graph)::edge_type;
    using Size = decltype(graph)::size_type;

    const auto vertices = {0U, 1U, 2U, 3U};
    const auto edges = {0U, 1U, 2U, 3U, 4U};

    CATCH_SECTION("{vertex,edge,size}_type")
    {
        CATCH_STATIC_REQUIRE((std::is_same_v<Vertex, std::size_t>));
        CATCH_STATIC_REQUIRE((std::is_same_v<Edge, std::size_t>));
        CATCH_STATIC_REQUIRE((std::is_same_v<Size, std::size_t>));
    }

    CATCH_SECTION("num_{vertices,edges}")
    {
        CATCH_CHECK(graph.num_vertices() == 4U);
        CATCH_CHECK(graph.num_edges() == 5U);
    }

    CATCH_SECTION("get_{vertex,edge}_id")
    {
        for (const auto& vertex : vertices) {
            CATCH_CHECK(graph.get_vertex_id(vertex) == vertex);
        }
        for (const auto& edge : edges) {
            CATCH_CHECK(graph.get_edge_id(edge) == edge);
        }
    }

    CATCH_SECTION("{vertices,edges}")
    {
        CATCH_CHECK_THAT(graph.vertices(), CM::RangeEquals(vertices));
        CATCH_CHECK_THAT(graph.edges(), CM::RangeEquals(edges));
    }

    CATCH_SECTION("contains_{vertex,edge}")
    {
        using ww::testing::ContainsVertex;
        CATCH_CHECK_THAT(graph, ContainsVertex(0U));
        CATCH_CHECK_THAT(graph, ContainsVertex(3U));
        CATCH_CHECK_THAT(graph, !ContainsVertex(999U));
        CATCH_CHECK_THAT(graph, !ContainsVertex(4U));

        using ww::testing::ContainsEdge;
        CATCH_CHECK_THAT(graph, ContainsEdge(0U));
        CATCH_CHECK_THAT(graph, ContainsEdge(4U));
        CATCH_CHECK_THAT(graph, !ContainsEdge(999U));
        CATCH_CHECK_THAT(graph, !ContainsEdge(5U));
    }

    CATCH_SECTION("outdegree")
    {
        CATCH_CHECK(graph.outdegree(0U) == 3U);
        CATCH_CHECK(graph.outdegree(1U) == 0U);
        CATCH_CHECK(graph.outdegree(2U) == 1U);
        CATCH_CHECK(graph.outdegree(3U) == 1U);
    }

    CATCH_SECTION("outgoing_edges")
    {
        using Pair = std::pair<Edge, Vertex>;
        const auto outgoing_edges = {Pair(0U, 1U), Pair(1U, 2U), Pair(2U, 3U)};
        CATCH_CHECK_THAT(graph.outgoing_edges(0U), CM::RangeEquals(outgoing_edges));
    }
}

CATCH_TEST_CASE("CSRGraph (nonconsecutive vertices)", "[graph]")
{
    auto edgelist = ww::EdgeList();
    edgelist.add_edge(0U, 1U);
    edgelist.add_edge(1U, 2U);
    edgelist.add_edge(4U, 5U);

    const auto graph = ww::CSRGraph(edgelist);

    CATCH_SECTION("num_{vertices,edges}")
    {
        CATCH_CHECK(graph.num_vertices() == 6U);
        CATCH_CHECK(graph.num_edges() == 3U);
    }

    CATCH_SECTION("{vertices,edges}")
    {
        const auto vertices = {0U, 1U, 2U, 3U, 4U, 5U};
        const auto edges = {0U, 1U, 2U};

        CATCH_CHECK_THAT(graph.vertices(), CM::RangeEquals(vertices));
        CATCH_CHECK_THAT(graph.edges(), CM::RangeEquals(edges));
    }

    CATCH_SECTION("contains_vertex")
    {
        using ww::testing::ContainsVertex;
        CATCH_CHECK_THAT(graph, ContainsVertex(3U));
        CATCH_CHECK_THAT(graph, !ContainsVertex(6U));
    }

    CATCH_SECTION("outdegree")
    {
        CATCH_CHECK(graph.outdegree(0U) == 1U);
        CATCH_CHECK(graph.outdegree(1U) == 1U);
        CATCH_CHECK(graph.outdegree(2U) == 0U);
        CATCH_CHECK(graph.outdegree(3U) == 0U);
        CATCH_CHECK(graph.outdegree(4U) == 1U);
        CATCH_CHECK(graph.outdegree(5U) == 0U);
    }
}

CATCH_TEST_CASE("CSRGraph.num_vertices", "[graph]")
{
    CATCH_SECTION("tail")
    {
        auto edgelist = ww::EdgeList();
        edgelist.add_edge(99U, 0U);
        const auto graph = ww::CSRGraph(edgelist);
        CATCH_CHECK(graph.num_vertices() == 100U);
    }

    CATCH_SECTION("head")
    {
        auto edgelist = ww::EdgeList();
        edgelist.add_edge(0U, 99U);
        const auto graph = ww::CSRGraph(edgelist);
        CATCH_CHECK(graph.num_vertices() == 100U);
    }
}

CATCH_TEST_CASE("CSRGraph (unsorted edges)", "[graph]")
{
    auto edgelist = ww::EdgeList();
    edgelist.add_edge(0U, 3U);
    edgelist.add_edge(2U, 1U);
    edgelist.add_edge(0U, 2U);
    edgelist.add_edge(3U, 0U);
    edgelist.add_edge(0U, 1U);

    const auto graph = ww::CSRGraph(edgelist);

    CATCH_SECTION("num_{vertices,edges}")
    {
        CATCH_CHECK(graph.num_vertices() == 4U);
        CATCH_CHECK(graph.num_edges() == 5U);
    }

    CATCH_SECTION("{vertices,edges}")
    {
        const auto vertices = {0U, 1U, 2U, 3U};
        const auto edges = {0U, 1U, 2U, 3U, 4U};

        CATCH_CHECK_THAT(graph.vertices(), CM::RangeEquals(vertices));
        CATCH_CHECK_THAT(graph.edges(), CM::RangeEquals(edges));
    }

    CATCH_SECTION("outgoing_edges")
    {
        using Edge = decltype(graph)::edge_type;
        using Vertex = decltype(graph)::vertex_type;
        using Pair = std::pair<Edge, Vertex>;
        const auto outgoing_edges = {Pair(0U, 1U), Pair(1U, 2U), Pair(2U, 3U)};

        CATCH_CHECK_THAT(graph.outgoing_edges(0U), CM::RangeEquals(outgoing_edges));
    }
}

CATCH_TEST_CASE("CSRGraph (parallel edges)", "[graph]")
{
    auto edgelist = ww::EdgeList();
    edgelist.add_edge(0U, 1U);
    edgelist.add_edge(0U, 1U);

    const auto graph = ww::CSRGraph(edgelist);

    CATCH_SECTION("num_{vertices,edges}")
    {
        CATCH_CHECK(graph.num_vertices() == 2U);
        CATCH_CHECK(graph.num_edges() == 2U);
    }

    CATCH_SECTION("outdegree")
    {
        CATCH_CHECK(graph.outdegree(0U) == 2U);
        CATCH_CHECK(graph.outdegree(1U) == 0U);
    }
}

CATCH_TEST_CASE("CSRGraph (self loops)", "[graph]")
{
    auto edgelist = ww::EdgeList();
    edgelist.add_edge(1U, 0U);
    edgelist.add_edge(1U, 1U);
    edgelist.add_edge(1U, 1U);
    edgelist.add_edge(1U, 2U);

    const auto graph = ww::CSRGraph(edgelist);

    CATCH_SECTION("num_{vertices,edges}")
    {
        CATCH_CHECK(graph.num_vertices() == 3U);
        CATCH_CHECK(graph.num_edges() == 4U);
    }

    CATCH_SECTION("contains_vertex")
    {
        using ww::testing::ContainsVertex;
        CATCH_CHECK_THAT(graph, ContainsVertex(0U));
        CATCH_CHECK_THAT(graph, ContainsVertex(2U));
    }

    CATCH_SECTION("outdegree") { CATCH_CHECK(graph.outdegree(1U) == 4U); }

    CATCH_SECTION("outgoing_edges")
    {
        using Edge = decltype(graph)::edge_type;
        using Vertex = decltype(graph)::vertex_type;
        using Pair = std::pair<Edge, Vertex>;
        const auto outgoing_edges = {Pair(0U, 0U), Pair(1U, 1U), Pair(2U, 1U),
                                     Pair(3U, 2U)};

        CATCH_CHECK_THAT(graph.outgoing_edges(1U), CM::RangeEquals(outgoing_edges));
    }
}

} // namespace
