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
        CHECK(graph.num_vertices() == 0);
        CHECK(graph.num_edges() == 0);
    }

    SECTION("contains_{vertex,edge}")
    {
        CHECK(!graph.contains_vertex(0));
        CHECK(!graph.contains_edge(0));
    }
}

TEST_CASE("CSRGraph", "[graph]")
{
    auto edgelist = ww::EdgeList();
    edgelist.add_edge(0, 1);
    edgelist.add_edge(0, 2);
    edgelist.add_edge(0, 3);
    edgelist.add_edge(2, 1);
    edgelist.add_edge(3, 0);

    const auto graph = ww::CSRGraph(edgelist);

    const auto vertices = {0, 1, 2, 3};
    const auto edges = {0, 1, 2, 3, 4};

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
        CHECK(graph.num_vertices() == 4);
        CHECK(graph.num_edges() == 5);
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
        CHECK(graph.contains_vertex(0));
        CHECK(graph.contains_vertex(3));
        CHECK(!graph.contains_vertex(-1));
        CHECK(!graph.contains_vertex(4));

        CHECK(graph.contains_edge(0));
        CHECK(graph.contains_edge(4));
        CHECK(!graph.contains_edge(-1));
        CHECK(!graph.contains_edge(5));
    }

    SECTION("outdegree")
    {
        CHECK(graph.outdegree(0) == 3);
        CHECK(graph.outdegree(1) == 0);
        CHECK(graph.outdegree(2) == 1);
        CHECK(graph.outdegree(3) == 1);
    }
}

TEST_CASE("CSRGraph (nonconsecutive vertices)", "[graph]")
{
    auto edgelist = ww::EdgeList();
    edgelist.add_edge(0, 1);
    edgelist.add_edge(1, 2);
    edgelist.add_edge(4, 5);

    const auto graph = ww::CSRGraph(edgelist);

    SECTION("num_{vertices,edges}")
    {
        CHECK(graph.num_vertices() == 6);
        CHECK(graph.num_edges() == 3);
    }

    SECTION("{vertices,edges}")
    {
        const auto vertices = {0, 1, 2, 3, 4, 5};
        CHECK(ranges::equal(graph.vertices(), vertices));

        const auto edges = {0, 1, 2};
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
    edgelist.add_edge(0, 3);
    edgelist.add_edge(2, 1);
    edgelist.add_edge(0, 2);
    edgelist.add_edge(3, 0);
    edgelist.add_edge(0, 1);

    const auto graph = ww::CSRGraph(edgelist);

    SECTION("num_{vertices,edges}")
    {
        CHECK(graph.num_vertices() == 4);
        CHECK(graph.num_edges() == 5);
    }

    SECTION("{vertices,edges}")
    {
        const auto vertices = {0, 1, 2, 3};
        CHECK(ranges::equal(graph.vertices(), vertices));

        const auto edges = {0, 1, 2, 3, 4};
        CHECK(ranges::equal(graph.edges(), edges));
    }
}

TEST_CASE("CSRGraph (parallel edges)", "[graph]")
{
    auto edgelist = ww::EdgeList();
    edgelist.add_edge(0, 1);
    edgelist.add_edge(0, 1);

    const auto graph = ww::CSRGraph(edgelist);

    SECTION("num_{vertices,edges}")
    {
        CHECK(graph.num_vertices() == 2);
        CHECK(graph.num_edges() == 2);
    }

    SECTION("outdegree") { CHECK(graph.outdegree(0) == 2); }
}

TEST_CASE("CSRGraph (self loops)", "[graph]")
{
    auto edgelist = ww::EdgeList();
    edgelist.add_edge(1, 0);
    edgelist.add_edge(1, 1);
    edgelist.add_edge(1, 1);
    edgelist.add_edge(1, 2);

    const auto graph = ww::CSRGraph(edgelist);

    SECTION("num_{vertices,edges}")
    {
        CHECK(graph.num_vertices() == 3);
        CHECK(graph.num_edges() == 4);
    }

    SECTION("contains_vertex")
    {
        CHECK(graph.contains_vertex(0));
        CHECK(graph.contains_vertex(2));
    }

    SECTION("outdegree") { CHECK(graph.outdegree(1) == 4); }
}

} // namespace
