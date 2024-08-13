#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_quantifiers.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <range/v3/view/transform.hpp>

#include <whirlwind/graph/csr_graph.hpp>
#include <whirlwind/graph/edge_list.hpp>
#include <whirlwind/graph/forest.hpp>
#include <whirlwind/graph/rectangular_grid_graph.hpp>

#include "../testing/matchers/forest_matchers.hpp"
#include "../testing/string_conversions.hpp" // IWYU pragma: keep

namespace {

namespace CM = Catch::Matchers;
namespace ww = whirlwind;

CATCH_TEST_CASE("Forest (const)", "[graph]")
{
    const auto graph = ww::RectangularGridGraph<>(4U, 4U);
    const auto forest = ww::Forest(graph);

    using Graph = decltype(forest)::graph_type;
    using Vertex = decltype(forest)::vertex_type;
    using Edge = decltype(forest)::edge_type;
    using Pred = decltype(forest)::pred_type;

    CATCH_SECTION("{graph,vertex,edge,pred}_type")
    {
        CATCH_STATIC_REQUIRE((std::is_same_v<Graph, ww::RectangularGridGraph<>>));
        CATCH_STATIC_REQUIRE((std::is_same_v<Vertex, Graph::vertex_type>));
        CATCH_STATIC_REQUIRE((std::is_same_v<Edge, Graph::edge_type>));
        CATCH_STATIC_REQUIRE((std::is_same_v<Pred, std::pair<Vertex, Edge>>));
    }

    CATCH_SECTION("graph")
    {
        CATCH_CHECK(std::addressof(forest.graph()) == std::addressof(graph));
    }

    CATCH_SECTION("predecessor_vertex")
    {
        const auto pred_vertices =
                graph.vertices() | ranges::views::transform([&](const auto& vertex) {
                    return forest.predecessor_vertex(vertex);
                });
        CATCH_CHECK_THAT(pred_vertices, CM::RangeEquals(graph.vertices()));
    }

    CATCH_SECTION("is_root_vertex")
    {
        using ww::testing::IsRootVertexIn;
        CATCH_CHECK_THAT(graph.vertices(), CM::AllMatch(IsRootVertexIn(forest)));
    }

    CATCH_SECTION("edge_fill_value")
    {
        CATCH_CHECK(forest.edge_fill_value() == Edge{});
    }
}

CATCH_TEST_CASE("Forest (non-const)", "[graph]")
{
    auto edgelist = ww::EdgeList();
    edgelist.add_edge(1U, 2U);
    edgelist.add_edge(2U, 3U);

    const auto graph = ww::CSRGraph(edgelist);
    auto forest = ww::Forest(graph);

    CATCH_SECTION("set_predecessor")
    {
        CATCH_CHECK(forest.predecessor_vertex(2U) == 2U);
        forest.set_predecessor(2U, 1U, 0U);
        CATCH_CHECK(forest.predecessor_vertex(2U) == 1U);
        CATCH_CHECK(forest.predecessor_edge(2U) == 0U);

        CATCH_CHECK(forest.predecessor_vertex(3U) == 3U);
        forest.set_predecessor(3U, {2U, 1U});
        CATCH_CHECK(forest.predecessor_vertex(3U) == 2U);
        CATCH_CHECK(forest.predecessor_edge(3U) == 1U);
    }

    CATCH_SECTION("make_root_vertex")
    {
        const auto vertex = 2U;
        const auto pred_vertex = 1U;
        const auto pred_edge = 0U;

        using ww::testing::IsRootVertexIn;
        CATCH_CHECK_THAT(vertex, IsRootVertexIn(forest));
        forest.set_predecessor(vertex, pred_vertex, pred_edge);
        CATCH_CHECK_THAT(vertex, !IsRootVertexIn(forest));
        forest.make_root_vertex(vertex);
        CATCH_CHECK_THAT(vertex, IsRootVertexIn(forest));
    }

    CATCH_SECTION("predecessors")
    {
        forest.set_predecessor(2U, 1U, 0U);
        forest.set_predecessor(3U, 2U, 1U);

        CATCH_CHECK(std::ranges::distance(forest.predecessors(0U)) == 0U);
        CATCH_CHECK(std::ranges::distance(forest.predecessors(1U)) == 0U);
        CATCH_CHECK(std::ranges::distance(forest.predecessors(2U)) == 1U);
        CATCH_CHECK(std::ranges::distance(forest.predecessors(3U)) == 2U);

        using Pred = decltype(forest)::pred_type;
        const auto preds = {Pred(2U, 1U), Pred(1U, 0U)};
        CATCH_CHECK_THAT(forest.predecessors(3U), CM::RangeEquals(preds));
    }

    CATCH_SECTION("reset")
    {
        forest.set_predecessor(2U, 1U, 0U);
        forest.set_predecessor(3U, 2U, 1U);

        using ww::testing::IsRootVertexIn;
        CATCH_CHECK_THAT(2U, !IsRootVertexIn(forest));
        CATCH_CHECK_THAT(3U, !IsRootVertexIn(forest));
        forest.reset();
        CATCH_CHECK_THAT(2U, IsRootVertexIn(forest));
        CATCH_CHECK_THAT(3U, IsRootVertexIn(forest));
    }
}

} // namespace
