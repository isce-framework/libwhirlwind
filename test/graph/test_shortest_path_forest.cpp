#include <iterator>
#include <limits>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_quantifiers.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <range/v3/view/transform.hpp>

#include <whirlwind/graph/csr_graph.hpp>
#include <whirlwind/graph/edge_list.hpp>
#include <whirlwind/graph/rectangular_grid_graph.hpp>
#include <whirlwind/graph/shortest_path_forest.hpp>

#include "../testing/matchers/forest_matchers.hpp"
#include "../testing/matchers/range_matchers.hpp"
#include "../testing/string_conversions.hpp" // IWYU pragma: keep

namespace {

namespace CM = Catch::Matchers;
namespace ww = whirlwind;

CATCH_TEST_CASE("ShortestPathForest (const)", "[graph]")
{
    using Distance = float;
    using Graph = ww::RectangularGridGraph<>;
    const auto graph = Graph(4U, 4U);
    const auto shortest_paths = ww::ShortestPathForest<Distance, Graph>(graph);

    using Distance_ = decltype(shortest_paths)::distance_type;
    using Graph_ = decltype(shortest_paths)::graph_type;
    using Vertex = decltype(shortest_paths)::vertex_type;
    using Edge = decltype(shortest_paths)::edge_type;

    CATCH_SECTION("{distance,graph,vertex,edge}_type")
    {
        CATCH_STATIC_REQUIRE((std::is_same_v<Distance_, Distance>));
        CATCH_STATIC_REQUIRE((std::is_same_v<Graph_, Graph>));
        CATCH_STATIC_REQUIRE((std::is_same_v<Vertex, Graph::vertex_type>));
        CATCH_STATIC_REQUIRE((std::is_same_v<Edge, Graph::edge_type>));
    }

    CATCH_SECTION("has_reached_vertex")
    {
        using ww::testing::WasReachedBy;
        CATCH_CHECK_THAT(graph.vertices(), CM::NoneMatch(WasReachedBy(shortest_paths)));
    }

    CATCH_SECTION("has_visited_vertex")
    {
        using ww::testing::WasVisitedBy;
        CATCH_CHECK_THAT(graph.vertices(), CM::NoneMatch(WasVisitedBy(shortest_paths)));
    }

    CATCH_SECTION("{reached,visited}_vertices")
    {
        CATCH_CHECK(std::ranges::distance(shortest_paths.reached_vertices()) == 0U);
        CATCH_CHECK(std::ranges::distance(shortest_paths.visited_vertices()) == 0U);
    }

    CATCH_SECTION("distance_to_vertex")
    {
        const auto distances =
                graph.vertices() | ranges::views::transform([&](const auto& vertex) {
                    return shortest_paths.distance_to_vertex(vertex);
                });
        constexpr auto inf = std::numeric_limits<Distance>::infinity();
        CATCH_CHECK_THAT(distances, ww::testing::AllEqualTo(inf));
    }
}

CATCH_TEST_CASE("ShortestPathForest (non-const)", "[graph]")
{
    auto edgelist = ww::EdgeList();
    edgelist.add_edge(0U, 1U);
    edgelist.add_edge(1U, 2U);

    using Distance = int;
    constexpr auto max_distance = std::numeric_limits<Distance>::max();

    const auto graph = ww::CSRGraph(edgelist);
    auto shortest_paths = ww::ShortestPathForest<Distance, decltype(graph)>(graph);

    CATCH_SECTION("label_vertex_reached")
    {
        using ww::testing::WasReachedBy;
        CATCH_CHECK_THAT(graph.vertices(), CM::NoneMatch(WasReachedBy(shortest_paths)));

        const auto vertices = {0U, 1U};
        for (const auto& vertex : vertices) {
            shortest_paths.label_vertex_reached(vertex);
        }

        CATCH_CHECK_THAT(vertices, CM::AllMatch(WasReachedBy(shortest_paths)));
        CATCH_CHECK_THAT(2U, !WasReachedBy(shortest_paths));
        CATCH_CHECK_THAT(shortest_paths.reached_vertices(), CM::RangeEquals(vertices));
    }

    CATCH_SECTION("label_vertex_visited")
    {
        using ww::testing::WasVisitedBy;
        CATCH_CHECK_THAT(graph.vertices(), CM::NoneMatch(WasVisitedBy(shortest_paths)));

        for (auto&& vertex : graph.vertices()) {
            shortest_paths.label_vertex_reached(vertex);
        }

        const auto vertices = {0U, 1U};
        CATCH_CHECK_THAT(vertices, NoneMatch(WasVisitedBy(shortest_paths)));

        for (const auto& vertex : vertices) {
            shortest_paths.label_vertex_visited(vertex);
        }

        CATCH_CHECK_THAT(vertices, CM::AllMatch(WasVisitedBy(shortest_paths)));
        CATCH_CHECK_THAT(2U, !WasVisitedBy(shortest_paths));
        CATCH_CHECK_THAT(shortest_paths.visited_vertices(), CM::RangeEquals(vertices));
    }

    CATCH_SECTION("set_distance_to_vertex")
    {
        CATCH_CHECK(shortest_paths.distance_to_vertex(0U) == max_distance);
        CATCH_CHECK(shortest_paths.distance_to_vertex(1U) == max_distance);
        CATCH_CHECK(shortest_paths.distance_to_vertex(2U) == max_distance);

        shortest_paths.set_distance_to_vertex(0U, 0);
        shortest_paths.set_distance_to_vertex(1U, 1);
        shortest_paths.set_distance_to_vertex(2U, 100);

        CATCH_CHECK(shortest_paths.distance_to_vertex(0U) == 0);
        CATCH_CHECK(shortest_paths.distance_to_vertex(1U) == 1);
        CATCH_CHECK(shortest_paths.distance_to_vertex(2U) == 100);
    }

    CATCH_SECTION("reset")
    {
        for (auto&& vertex : shortest_paths.graph().vertices()) {
            shortest_paths.label_vertex_reached(vertex);
            shortest_paths.set_distance_to_vertex(vertex, 100);
        }

        shortest_paths.label_vertex_visited(0U);
        shortest_paths.set_distance_to_vertex(0U, 0);

        shortest_paths.label_vertex_visited(1U);
        shortest_paths.set_distance_to_vertex(1U, 1);

        shortest_paths.reset();

        using ww::testing::WasReachedBy;
        using ww::testing::WasVisitedBy;
        CATCH_CHECK_THAT(graph.vertices(), CM::NoneMatch(WasReachedBy(shortest_paths)));
        CATCH_CHECK_THAT(graph.vertices(), CM::NoneMatch(WasVisitedBy(shortest_paths)));

        const auto distances =
                graph.vertices() | ranges::views::transform([&](const auto& vertex) {
                    return shortest_paths.distance_to_vertex(vertex);
                });
        CATCH_CHECK_THAT(distances, ww::testing::AllEqualTo(max_distance));
    }
}

} // namespace
