#include <memory>
#include <type_traits>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <range/v3/view/transform.hpp>

#include <whirlwind/graph/csr_graph.hpp>
#include <whirlwind/graph/edge_list.hpp>
#include <whirlwind/graph/forest.hpp>
#include <whirlwind/graph/rectangular_grid_graph.hpp>

#include "../string_conversions.hpp"

namespace {

namespace ww = whirlwind;

TEST_CASE("Forest (const)", "[graph]")
{
    const auto graph = ww::RectangularGridGraph<>(4U, 4U);
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
        CHECK_THAT(pred_vertices, Catch::Matchers::RangeEquals(graph.vertices()));
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
    edgelist.add_edge(1U, 2U);
    edgelist.add_edge(2U, 3U);

    const auto graph = ww::CSRGraph(edgelist);

    auto forest = ww::Forest(graph);

    SECTION("set_predecessor")
    {
        CHECK(forest.predecessor_vertex(2U) == 2U);
        forest.set_predecessor(2U, 1U, 0U);
        CHECK(forest.predecessor_vertex(2U) == 1U);
        CHECK(forest.predecessor_edge(2U) == 0U);

        CHECK(forest.predecessor_vertex(3U) == 3U);
        forest.set_predecessor(3U, {2U, 1U});
        CHECK(forest.predecessor_vertex(3U) == 2U);
        CHECK(forest.predecessor_edge(3U) == 1U);
    }

    SECTION("make_root_vertex")
    {
        CHECK(forest.is_root_vertex(2U));
        forest.set_predecessor(2U, 1U, 0U);
        CHECK_FALSE(forest.is_root_vertex(2U));
        forest.make_root_vertex(2U);
        CHECK(forest.is_root_vertex(2U));
    }

    SECTION("reset")
    {
        forest.set_predecessor(2U, 1U, 0U);
        forest.set_predecessor(3U, 2U, 1U);
        CHECK_FALSE(forest.is_root_vertex(2U));
        CHECK_FALSE(forest.is_root_vertex(3U));
        forest.reset();
        CHECK(forest.is_root_vertex(2U));
        CHECK(forest.is_root_vertex(3U));
    }
}

} // namespace
