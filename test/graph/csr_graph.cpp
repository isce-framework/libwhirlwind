#include <cstddef>
#include <type_traits>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include <whirlwind/graph/csr_graph.hpp>
#include <whirlwind/graph/edge_list.hpp>

#include "../testing/string_conversions.hpp" // IWYU pragma: keep

namespace {

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
        CATCH_CHECK_FALSE(graph.contains_vertex(0U));
        CATCH_CHECK_FALSE(graph.contains_edge(0U));
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
        using Catch::Matchers::RangeEquals;
        CATCH_CHECK_THAT(graph.vertices(), RangeEquals(vertices));
        CATCH_CHECK_THAT(graph.edges(), RangeEquals(edges));
    }

    CATCH_SECTION("contains_{vertex,edge}")
    {
        CATCH_CHECK(graph.contains_vertex(0U));
        CATCH_CHECK(graph.contains_vertex(3U));
        CATCH_CHECK_FALSE(graph.contains_vertex(999U));
        CATCH_CHECK_FALSE(graph.contains_vertex(4U));

        CATCH_CHECK(graph.contains_edge(0U));
        CATCH_CHECK(graph.contains_edge(4U));
        CATCH_CHECK_FALSE(graph.contains_edge(999U));
        CATCH_CHECK_FALSE(graph.contains_edge(5U));
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
        using Catch::Matchers::RangeEquals;
        CATCH_CHECK_THAT(graph.outgoing_edges(0U), RangeEquals(outgoing_edges));
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
        using Catch::Matchers::RangeEquals;

        const auto vertices = {0U, 1U, 2U, 3U, 4U, 5U};
        CATCH_CHECK_THAT(graph.vertices(), RangeEquals(vertices));

        const auto edges = {0U, 1U, 2U};
        CATCH_CHECK_THAT(graph.edges(), RangeEquals(edges));
    }

    CATCH_SECTION("contains_vertex")
    {
        CATCH_CHECK(graph.contains_vertex(3U));
        CATCH_CHECK_FALSE(graph.contains_vertex(6U));
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
        using Catch::Matchers::RangeEquals;

        const auto vertices = {0U, 1U, 2U, 3U};
        CATCH_CHECK_THAT(graph.vertices(), RangeEquals(vertices));

        const auto edges = {0U, 1U, 2U, 3U, 4U};
        CATCH_CHECK_THAT(graph.edges(), RangeEquals(edges));
    }

    CATCH_SECTION("outgoing_edges")
    {
        using Edge = decltype(graph)::edge_type;
        using Vertex = decltype(graph)::vertex_type;
        using Pair = std::pair<Edge, Vertex>;
        const auto outgoing_edges = {Pair(0U, 1U), Pair(1U, 2U), Pair(2U, 3U)};

        using Catch::Matchers::RangeEquals;
        CATCH_CHECK_THAT(graph.outgoing_edges(0U), RangeEquals(outgoing_edges));
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
        CATCH_CHECK(graph.contains_vertex(0U));
        CATCH_CHECK(graph.contains_vertex(2U));
    }

    CATCH_SECTION("outdegree") { CATCH_CHECK(graph.outdegree(1U) == 4U); }

    CATCH_SECTION("outgoing_edges")
    {
        using Edge = decltype(graph)::edge_type;
        using Vertex = decltype(graph)::vertex_type;
        using Pair = std::pair<Edge, Vertex>;
        const auto outgoing_edges = {Pair(0U, 0U), Pair(1U, 1U), Pair(2U, 1U),
                                     Pair(3U, 2U)};

        using Catch::Matchers::RangeEquals;
        CATCH_CHECK_THAT(graph.outgoing_edges(1U), RangeEquals(outgoing_edges));
    }
}

} // namespace
