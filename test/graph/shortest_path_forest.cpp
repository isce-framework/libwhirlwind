#include <cmath>
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
#include <whirlwind/graph/forest_concepts.hpp>
#include <whirlwind/graph/rectangular_grid_graph.hpp>
#include <whirlwind/graph/shortest_path_forest.hpp>

namespace {

namespace ww = whirlwind;

[[nodiscard]] constexpr auto
is_each_vertex_unreached(const ww::ShortestPathForestType auto& shortest_paths)
{
    return shortest_paths.graph().vertices() |
           ranges::views::transform([&](const auto& vertex) {
               return !shortest_paths.has_reached_vertex(vertex);
           });
}

template<class ShortestPathForest>
[[nodiscard]] constexpr auto
is_each_vertex_unvisited(const ShortestPathForest& shortest_paths)
{
    return shortest_paths.graph().vertices() |
           ranges::views::transform([&](const auto& vertex) {
               return !shortest_paths.has_visited_vertex(vertex);
           });
}

CATCH_TEST_CASE("ShortestPathForest (const)", "[graph]")
{
    using D = float;
    using G = ww::RectangularGridGraph<>;
    const auto graph = G(4U, 4U);
    const auto shortest_paths = ww::ShortestPathForest<D, G>(graph);

    using Distance = decltype(shortest_paths)::distance_type;
    using Graph = decltype(shortest_paths)::graph_type;
    using Vertex = decltype(shortest_paths)::vertex_type;
    using Edge = decltype(shortest_paths)::edge_type;

    CATCH_SECTION("{distance,graph,vertex,edge}_type")
    {
        CATCH_STATIC_REQUIRE((std::is_same_v<Distance, D>));
        CATCH_STATIC_REQUIRE((std::is_same_v<Graph, G>));
        CATCH_STATIC_REQUIRE((std::is_same_v<Vertex, G::vertex_type>));
        CATCH_STATIC_REQUIRE((std::is_same_v<Edge, G::edge_type>));
    }

    CATCH_SECTION("has_reached_vertex")
    {
        using Catch::Matchers::AllTrue;
        CATCH_CHECK_THAT(is_each_vertex_unreached(shortest_paths), AllTrue());
    }

    CATCH_SECTION("has_visited_vertex")
    {
        using Catch::Matchers::AllTrue;
        CATCH_CHECK_THAT(is_each_vertex_unvisited(shortest_paths), AllTrue());
    }

    CATCH_SECTION("{reached,visited}_vertices")
    {
        CATCH_CHECK(std::ranges::distance(shortest_paths.reached_vertices()) == 0U);
        CATCH_CHECK(std::ranges::distance(shortest_paths.visited_vertices()) == 0U);
    }

    CATCH_SECTION("distance_to_vertex")
    {
        const auto distances_are_inf =
                shortest_paths.graph().vertices() |
                ranges::views::transform([&](const auto& vertex) {
                    return std::isinf(shortest_paths.distance_to_vertex(vertex));
                });
        CATCH_CHECK_THAT(distances_are_inf, Catch::Matchers::AllTrue());
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
        CATCH_CHECK_FALSE(shortest_paths.has_reached_vertex(0U));
        CATCH_CHECK_FALSE(shortest_paths.has_reached_vertex(1U));

        shortest_paths.label_vertex_reached(0U);
        shortest_paths.label_vertex_reached(1U);

        CATCH_CHECK(shortest_paths.has_reached_vertex(0U));
        CATCH_CHECK(shortest_paths.has_reached_vertex(1U));

        const auto reached = {0U, 1U};
        using Catch::Matchers::RangeEquals;
        CATCH_CHECK_THAT(shortest_paths.reached_vertices(), RangeEquals(reached));
    }

    CATCH_SECTION("label_vertex_visited")
    {
        CATCH_CHECK_FALSE(shortest_paths.has_visited_vertex(0U));
        CATCH_CHECK_FALSE(shortest_paths.has_visited_vertex(1U));

        for (auto&& vertex : shortest_paths.graph().vertices()) {
            shortest_paths.label_vertex_reached(vertex);
        }

        CATCH_CHECK_FALSE(shortest_paths.has_visited_vertex(0U));
        CATCH_CHECK_FALSE(shortest_paths.has_visited_vertex(1U));

        shortest_paths.label_vertex_visited(0U);
        shortest_paths.label_vertex_visited(1U);

        CATCH_CHECK(shortest_paths.has_visited_vertex(0U));
        CATCH_CHECK(shortest_paths.has_visited_vertex(1U));

        const auto visited = {0U, 1U};
        using Catch::Matchers::RangeEquals;
        CATCH_CHECK_THAT(shortest_paths.visited_vertices(), RangeEquals(visited));
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

        using Catch::Matchers::AllTrue;
        CATCH_CHECK_THAT(is_each_vertex_unreached(shortest_paths), AllTrue());
        CATCH_CHECK_THAT(is_each_vertex_unvisited(shortest_paths), AllTrue());

        const auto distances_are_max =
                shortest_paths.graph().vertices() |
                ranges::views::transform([&](const auto& vertex) {
                    return shortest_paths.distance_to_vertex(vertex) == max_distance;
                });
        CATCH_CHECK_THAT(distances_are_max, AllTrue());
    }
}

} // namespace
