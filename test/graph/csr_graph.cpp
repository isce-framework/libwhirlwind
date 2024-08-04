#include <cstddef>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <range/v3/algorithm/equal.hpp>

#include <whirlwind/graph/csr_graph.hpp>
#include <whirlwind/graph/edge_list.hpp>

namespace {

namespace ww = whirlwind;

TEST_CASE("CSRGraph (empty)", "[graph]")
{
    const auto graph = ww::CSRGraph();

    SECTION("num_{vertices,edges}")
    {
        CHECK(graph.num_vertices() == 0U);
        CHECK(graph.num_edges() == 0U);
    }

    SECTION("contains_{vertex,edge}")
    {
        CHECK(!graph.contains_vertex(0U));
        CHECK(!graph.contains_edge(0U));
    }
}

TEST_CASE("CSRGraph", "[graph]")
{
    auto edgelist = ww::EdgeList();
    edgelist.add_edge(0U, 1U);
    edgelist.add_edge(0U, 2U);
    edgelist.add_edge(0U, 3U);
    edgelist.add_edge(2U, 1U);
    edgelist.add_edge(3U, 0U);

    const auto graph = ww::CSRGraph(edgelist);

    const auto vertices = {0U, 1U, 2U, 3U};
    const auto edges = {0U, 1U, 2U, 3U, 4U};

    SECTION("{vertex,edge,size}_type")
    {
        using Vertex = typename decltype(graph)::vertex_type;
        STATIC_REQUIRE((std::is_same_v<Vertex, std::size_t>));

        using Edge = typename decltype(graph)::edge_type;
        STATIC_REQUIRE((std::is_same_v<Edge, std::size_t>));

        using Size = typename decltype(graph)::size_type;
        STATIC_REQUIRE((std::is_same_v<Size, std::size_t>));
    }

    SECTION("num_{vertices,edges}")
    {
        CHECK(graph.num_vertices() == 4U);
        CHECK(graph.num_edges() == 5U);
    }

    SECTION("get_{vertex,edge}_id")
    {
        for (const auto& vertex : vertices) {
            CHECK(graph.get_vertex_id(vertex) == vertex);
        }
        for (const auto& edge : edges) {
            CHECK(graph.get_edge_id(edge) == edge);
        }
    }

    SECTION("{vertices,edges}")
    {
        CHECK(ranges::equal(graph.vertices(), vertices));
        CHECK(ranges::equal(graph.edges(), edges));
    }

    SECTION("contains_{vertex,edge}")
    {
        CHECK(graph.contains_vertex(0U));
        CHECK(graph.contains_vertex(3U));
        CHECK(!graph.contains_vertex(999U));
        CHECK(!graph.contains_vertex(4U));

        CHECK(graph.contains_edge(0U));
        CHECK(graph.contains_edge(4U));
        CHECK(!graph.contains_edge(999U));
        CHECK(!graph.contains_edge(5U));
    }

    SECTION("outdegree")
    {
        CHECK(graph.outdegree(0U) == 3U);
        CHECK(graph.outdegree(1U) == 0U);
        CHECK(graph.outdegree(2U) == 1U);
        CHECK(graph.outdegree(3U) == 1U);
    }
}

TEST_CASE("CSRGraph (nonconsecutive vertices)", "[graph]")
{
    auto edgelist = ww::EdgeList();
    edgelist.add_edge(0U, 1U);
    edgelist.add_edge(1U, 2U);
    edgelist.add_edge(4U, 5U);

    const auto graph = ww::CSRGraph(edgelist);

    SECTION("num_{vertices,edges}")
    {
        CHECK(graph.num_vertices() == 6U);
        CHECK(graph.num_edges() == 3U);
    }

    SECTION("{vertices,edges}")
    {
        const auto vertices = {0U, 1U, 2U, 3U, 4U, 5U};
        CHECK(ranges::equal(graph.vertices(), vertices));

        const auto edges = {0U, 1U, 2U};
        CHECK(ranges::equal(graph.edges(), edges));
    }

    SECTION("contains_vertex")
    {
        CHECK(graph.contains_vertex(3));
        CHECK(!graph.contains_vertex(6));
    }

    SECTION("outdegree") { CHECK(graph.outdegree(3) == 0); }
}

TEST_CASE("CSRGraph (unsorted edges)", "[graph]")
{
    auto edgelist = ww::EdgeList();
    edgelist.add_edge(0U, 3U);
    edgelist.add_edge(2U, 1U);
    edgelist.add_edge(0U, 2U);
    edgelist.add_edge(3U, 0U);
    edgelist.add_edge(0U, 1U);

    const auto graph = ww::CSRGraph(edgelist);

    SECTION("num_{vertices,edges}")
    {
        CHECK(graph.num_vertices() == 4U);
        CHECK(graph.num_edges() == 5U);
    }

    SECTION("{vertices,edges}")
    {
        const auto vertices = {0U, 1U, 2U, 3U};
        CHECK(ranges::equal(graph.vertices(), vertices));

        const auto edges = {0U, 1U, 2U, 3U, 4U};
        CHECK(ranges::equal(graph.edges(), edges));
    }
}

TEST_CASE("CSRGraph (parallel edges)", "[graph]")
{
    auto edgelist = ww::EdgeList();
    edgelist.add_edge(0U, 1U);
    edgelist.add_edge(0U, 1U);

    const auto graph = ww::CSRGraph(edgelist);

    SECTION("num_{vertices,edges}")
    {
        CHECK(graph.num_vertices() == 2U);
        CHECK(graph.num_edges() == 2U);
    }

    SECTION("outdegree") { CHECK(graph.outdegree(0) == 2U); }
}

TEST_CASE("CSRGraph (self loops)", "[graph]")
{
    auto edgelist = ww::EdgeList();
    edgelist.add_edge(1U, 0U);
    edgelist.add_edge(1U, 1U);
    edgelist.add_edge(1U, 1U);
    edgelist.add_edge(1U, 2U);

    const auto graph = ww::CSRGraph(edgelist);

    SECTION("num_{vertices,edges}")
    {
        CHECK(graph.num_vertices() == 3U);
        CHECK(graph.num_edges() == 4U);
    }

    SECTION("contains_vertex")
    {
        CHECK(graph.contains_vertex(0U));
        CHECK(graph.contains_vertex(2U));
    }

    SECTION("outdegree") { CHECK(graph.outdegree(1U) == 4U); }
}

} // namespace
