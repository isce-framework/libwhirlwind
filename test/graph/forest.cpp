#include <memory>
#include <type_traits>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/view/transform.hpp>

#include <whirlwind/graph/csr_graph.hpp>
#include <whirlwind/graph/edge_list.hpp>
#include <whirlwind/graph/forest.hpp>
#include <whirlwind/graph/rectangular_grid_graph.hpp>

namespace {

namespace ww = whirlwind;

TEST_CASE("Forest (const)", "[graph]")
{
    const auto graph = ww::RectangularGridGraph<>(4, 4);
    const auto forest = ww::Forest(graph);

    SECTION("{graph,vertex,edge,pred}_type")
    {
        using Graph = typename decltype(forest)::graph_type;
        STATIC_REQUIRE((std::is_same_v<Graph, ww::RectangularGridGraph<>>));

        using Vertex = typename decltype(forest)::vertex_type;
        STATIC_REQUIRE((std::is_same_v<Vertex, Graph::vertex_type>));

        using Edge = typename decltype(forest)::edge_type;
        STATIC_REQUIRE((std::is_same_v<Edge, Graph::edge_type>));

        using Pred = typename decltype(forest)::pred_type;
        STATIC_REQUIRE((std::is_same_v<Pred, std::pair<Vertex, Edge>>));
    }

    SECTION("graph") { CHECK(std::addressof(forest.graph()) == std::addressof(graph)); }

    SECTION("predecessor_vertex")
    {
        const auto pred_vertices =
                graph.vertices() | ranges::views::transform([&](const auto& vertex) {
                    return forest.predecessor_vertex(vertex);
                });
        CHECK(ranges::equal(pred_vertices, graph.vertices()));
    }

    SECTION("edge_fill_value")
    {
        using Edge = typename decltype(forest)::edge_type;
        CHECK(forest.edge_fill_value() == Edge{});
    }
}

TEST_CASE("Forest (non-const)", "[graph]")
{
    auto edgelist = ww::EdgeList();
    edgelist.add_edge(1, 2);
    edgelist.add_edge(2, 3);

    const auto graph = ww::CSRGraph(edgelist);

    auto forest = ww::Forest(graph);

    SECTION("set_predecessor")
    {
        CHECK(forest.predecessor_vertex(2) == 2);
        forest.set_predecessor(2, 1, 0);
        CHECK(forest.predecessor_vertex(2) == 1);
        CHECK(forest.predecessor_edge(2) == 0);

        CHECK(forest.predecessor_vertex(3) == 3);
        forest.set_predecessor(3, {2, 1});
        CHECK(forest.predecessor_vertex(3) == 2);
        CHECK(forest.predecessor_edge(3) == 1);
    }

    SECTION("make_root_vertex")
    {
        CHECK(forest.is_root_vertex(2));
        forest.set_predecessor(2, 1, 0);
        CHECK(!forest.is_root_vertex(2));
        forest.make_root_vertex(2);
        CHECK(forest.is_root_vertex(2));
    }

    SECTION("reset")
    {
        forest.set_predecessor(2, 1, 0);
        forest.set_predecessor(3, 2, 1);
        CHECK(!forest.is_root_vertex(2));
        CHECK(!forest.is_root_vertex(3));
        forest.reset();
        CHECK(forest.is_root_vertex(2));
        CHECK(forest.is_root_vertex(3));
    }
}

} // namespace
