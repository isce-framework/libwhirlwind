#include <limits>
#include <memory>
#include <type_traits>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_container_properties.hpp>
#include <catch2/matchers/catch_matchers_contains.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_quantifiers.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>

#include <whirlwind/graph/csr_graph.hpp>
#include <whirlwind/graph/dijkstra.hpp>
#include <whirlwind/graph/edge_list.hpp>

#include "../testing/matchers/forest_matchers.hpp"
#include "../testing/matchers/range_matchers.hpp"

namespace {

namespace CM = Catch::Matchers;
namespace ww = whirlwind;

CATCH_TEST_CASE("Dijkstra", "[graph]")
{
    using Distance = int;
    using Graph = ww::CSRGraph<>;

    constexpr auto max_distance = std::numeric_limits<Distance>::max();

    auto edgelist = ww::EdgeList();
    edgelist.add_edge(0U, 1U);
    edgelist.add_edge(1U, 2U);
    edgelist.add_edge(2U, 3U);

    const auto graph = Graph(edgelist);

    auto dijkstra = ww::Dijkstra<Distance, Graph>(graph);

    using Distance_ = decltype(dijkstra)::distance_type;
    using Graph_ = decltype(dijkstra)::graph_type;
    using Vertex = decltype(dijkstra)::vertex_type;
    using Edge = decltype(dijkstra)::edge_type;

    CATCH_SECTION("Dijkstra")
    {
        CATCH_CHECK(std::addressof(dijkstra.graph()) == std::addressof(graph));
        CATCH_CHECK_THAT(dijkstra.heap(), CM::IsEmpty());

        CATCH_CHECK(dijkstra.done());

        using ww::testing::WasReachedBy;
        CATCH_CHECK_THAT(graph.vertices(), CM::NoneMatch(WasReachedBy(dijkstra)));

        const auto distances =
                graph.vertices() | ranges::views::transform([&](const auto& vertex) {
                    return dijkstra.distance_to_vertex(vertex);
                });
        CATCH_CHECK_THAT(distances, ww::testing::AllEqualTo(max_distance));
    }

    CATCH_SECTION("{distance,graph,vertex,edge}_type")
    {
        CATCH_STATIC_REQUIRE((std::is_same_v<Distance_, Distance>));
        CATCH_STATIC_REQUIRE((std::is_same_v<Graph_, Graph>));
        CATCH_STATIC_REQUIRE((std::is_same_v<Vertex, Graph::vertex_type>));
        CATCH_STATIC_REQUIRE((std::is_same_v<Edge, Graph::edge_type>));
    }

    CATCH_SECTION("heap")
    {
        const auto source = 0U;
        dijkstra.add_source(source);

        const auto edges = {0U, 1U, 2U};
        const auto heads = {1U, 2U, 3U};
        const auto lengths = {1, 10, 100};

        auto tail = source;
        auto total_distance = 0;
        for (auto&& [edge, head, length] : ranges::views::zip(edges, heads, lengths)) {
            dijkstra.visit_vertex(tail, total_distance);
            total_distance += length;
            dijkstra.relax_edge(edge, tail, head, total_distance);
            tail = head;
        }

        CATCH_CHECK(std::size(dijkstra.heap()) == 4U);

        const auto vertices = {0U, 1U, 2U, 3U};
        const auto distances = {0, 1, 11, 111};
        for (auto&& [v, d] : ranges::views::zip(vertices, distances)) {
            const auto [vertex, distance] = dijkstra.heap().top();
            CATCH_CHECK(vertex == v);
            CATCH_CHECK(distance == d);
            dijkstra.heap().pop();
        }

        CATCH_CHECK_THAT(dijkstra.heap(), CM::IsEmpty());
    }

    CATCH_SECTION("pop_next_unvisited_vertex")
    {
        const auto vertex = 0U;
        const auto distance = 0;
        dijkstra.add_source(vertex);
        CATCH_CHECK(std::size(dijkstra.heap()) == 1U);
        const auto& [vertex1, distance1] = dijkstra.heap().top();
        CATCH_CHECK(vertex1 == vertex);
        CATCH_CHECK(distance1 == distance);

        const auto [vertex2, distance2] = dijkstra.pop_next_unvisited_vertex();
        CATCH_CHECK_THAT(dijkstra.heap(), CM::IsEmpty());
        CATCH_CHECK(vertex2 == vertex);
        CATCH_CHECK(distance2 == distance);
    }

    CATCH_SECTION("add_source")
    {
        const auto sources = {0U, 1U};
        for (const auto& source : sources) {
            dijkstra.add_source(source);
        }

        CATCH_CHECK(std::size(dijkstra.heap()) == std::size(sources));
        CATCH_CHECK_THAT(sources, CM::AllMatch(ww::testing::WasReachedBy(dijkstra)));

        const auto [vertex0, distance0] = dijkstra.pop_next_unvisited_vertex();
        CATCH_CHECK_THAT(sources, CM::Contains(vertex0));
        CATCH_CHECK(distance0 == 0);

        const auto [vertex1, distance1] = dijkstra.pop_next_unvisited_vertex();
        CATCH_CHECK_THAT(sources, CM::Contains(vertex1));
        CATCH_CHECK(distance1 == 0);

        CATCH_CHECK_THAT(dijkstra.heap(), CM::IsEmpty());
        CATCH_CHECK(vertex0 != vertex1);
    }

    CATCH_SECTION("visit_vertex")
    {
        const auto vertex0 = 0U;
        const auto distance0 = 0;
        dijkstra.add_source(vertex0);

        using ww::testing::WasVisitedBy;
        CATCH_CHECK_THAT(vertex0, !WasVisitedBy(dijkstra));
        dijkstra.visit_vertex(vertex0, distance0);
        CATCH_CHECK_THAT(vertex0, WasVisitedBy(dijkstra));
        CATCH_CHECK(dijkstra.distance_to_vertex(vertex0) == distance0);

        const auto edge = 0U;
        const auto vertex1 = 1U;
        const auto distance1 = 10;
        dijkstra.relax_edge(edge, vertex0, vertex1, distance1);
        CATCH_CHECK_THAT(vertex1, !WasVisitedBy(dijkstra));

        dijkstra.visit_vertex(vertex1, distance1);
        CATCH_CHECK_THAT(vertex1, WasVisitedBy(dijkstra));
        CATCH_CHECK(dijkstra.distance_to_vertex(vertex1) == distance1);
    }

    CATCH_SECTION("relax_edge")
    {
        const auto source = 0U;
        dijkstra.add_source(source);

        const auto [tail, distance] = dijkstra.pop_next_unvisited_vertex();
        dijkstra.visit_vertex(tail, distance);

        const auto edge = 0U;
        const auto head = 1U;
        const auto length = 10;
        dijkstra.relax_edge(edge, tail, head, distance + length);

        CATCH_CHECK_THAT(head, ww::testing::WasReachedBy(dijkstra));
        CATCH_CHECK_THAT(head, !ww::testing::WasVisitedBy(dijkstra));
        CATCH_CHECK(dijkstra.distance_to_vertex(head) == distance + length);

        CATCH_CHECK(std::size(dijkstra.heap()) == 1U);
        const auto [vertex, distance1] = dijkstra.heap().top();
        CATCH_CHECK(vertex == head);
        CATCH_CHECK(distance1 == distance + length);
    }

    CATCH_SECTION("done")
    {
        CATCH_CHECK(dijkstra.done());
        dijkstra.add_source(0U);
        CATCH_CHECK_FALSE(dijkstra.done());
        dijkstra.pop_next_unvisited_vertex();
        CATCH_CHECK(dijkstra.done());
    }

    CATCH_SECTION("reset")
    {
        const auto source = 0U;
        dijkstra.add_source(source);

        const auto edges = {0U, 1U, 2U};
        const auto heads = {1U, 2U, 3U};
        const auto lengths = {1, 10, 100};

        auto tail = source;
        auto total_distance = 0;
        for (auto&& [edge, head, length] : ranges::views::zip(edges, heads, lengths)) {
            dijkstra.visit_vertex(tail, total_distance);
            total_distance += length;
            dijkstra.relax_edge(edge, tail, head, total_distance);
            tail = head;
        }

        dijkstra.reset();

        CATCH_CHECK_THAT(dijkstra.heap(), CM::IsEmpty());
        CATCH_CHECK(dijkstra.done());

        using ww::testing::WasReachedBy;
        CATCH_CHECK_THAT(graph.vertices(), CM::NoneMatch(WasReachedBy(dijkstra)));

        const auto distances =
                graph.vertices() | ranges::views::transform([&](const auto& vertex) {
                    return dijkstra.distance_to_vertex(vertex);
                });
        CATCH_CHECK_THAT(distances, ww::testing::AllEqualTo(max_distance));
    }
}

CATCH_TEST_CASE("Dijkstra (sorted)", "[graph]")
{
    using Distance = double;
    using Graph = ww::CSRGraph<>;

    const auto tail = 0U;
    const auto edges = {0U, 1U, 2U, 3U};
    const auto heads = {1U, 2U, 3U, 4U};
    const auto lengths = {100.0, 1.0, 1000.0, 10.0};

    auto edgelist = ww::EdgeList();
    for (const auto& head : heads) {
        edgelist.add_edge(tail, head);
    }

    const auto graph = Graph(edgelist);

    auto dijkstra = ww::Dijkstra<Distance, Graph>(graph);

    dijkstra.add_source(tail);
    dijkstra.pop_next_unvisited_vertex();
    dijkstra.visit_vertex(tail, 0.0);
    for (auto&& [edge, head, length] : ranges::views::zip(edges, heads, lengths)) {
        dijkstra.relax_edge(edge, tail, head, length);
    }

    const auto vertices = {2U, 4U, 1U, 3U};
    const auto distances = {1.0, 10.0, 100.0, 1000.0};
    for (auto&& [v, d] : ranges::views::zip(vertices, distances)) {
        const auto [vertex, distance] = dijkstra.pop_next_unvisited_vertex();
        CATCH_CHECK(vertex == v);
        CATCH_CHECK_THAT(distance, CM::WithinAbs(d, 1e-12));
    }
}

CATCH_TEST_CASE("Dijkstra.done (visited)", "[graph]")
{
    const auto source = 0U;

    auto edgelist = ww::EdgeList();
    edgelist.add_edge(source, source);

    const auto graph = ww::CSRGraph(edgelist);

    auto dijkstra = ww::Dijkstra<int, decltype(graph)>(graph);

    CATCH_CHECK(dijkstra.done());
    dijkstra.add_source(source);
    CATCH_CHECK_FALSE(dijkstra.done());
    dijkstra.visit_vertex(source, 0);
    CATCH_CHECK(dijkstra.done());
}

} // namespace
