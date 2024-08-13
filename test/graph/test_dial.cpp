#include <cstddef>
#include <limits>
#include <memory>
#include <type_traits>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_container_properties.hpp>
#include <catch2/matchers/catch_matchers_contains.hpp>
#include <catch2/matchers/catch_matchers_quantifiers.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>

#include <whirlwind/graph/csr_graph.hpp>
#include <whirlwind/graph/dial.hpp>
#include <whirlwind/graph/edge_list.hpp>
#include <whirlwind/graph/rectangular_grid_graph.hpp>

#include "../testing/matchers/forest_matchers.hpp"
#include "../testing/matchers/range_matchers.hpp"

namespace {

namespace CM = Catch::Matchers;
namespace ww = whirlwind;

CATCH_TEST_CASE("Dial", "[graph]")
{
    using Distance = int;
    using Graph = ww::CSRGraph<>;

    constexpr auto max_distance = std::numeric_limits<Distance>::max();

    auto edgelist = ww::EdgeList();
    edgelist.add_edge(0U, 1U);
    edgelist.add_edge(1U, 2U);
    edgelist.add_edge(2U, 3U);

    const auto graph = Graph(edgelist);
    const auto num_buckets = 101U;

    auto dial = ww::Dial<Distance, Graph>(graph, num_buckets);

    using Distance_ = decltype(dial)::distance_type;
    using Graph_ = decltype(dial)::graph_type;
    using Vertex = decltype(dial)::vertex_type;
    using Edge = decltype(dial)::edge_type;
    using Size = decltype(dial)::size_type;

    CATCH_SECTION("Dial")
    {
        CATCH_CHECK(std::addressof(dial.graph()) == std::addressof(graph));
        CATCH_CHECK(dial.num_buckets() == num_buckets);

        CATCH_CHECK(std::size(dial.buckets()) == num_buckets);
        CATCH_CHECK_THAT(dial.buckets(), CM::AllMatch(CM::IsEmpty()));
        CATCH_CHECK(dial.current_bucket_id() == 0U);

        CATCH_CHECK(dial.done());

        using ww::testing::WasReachedBy;
        CATCH_CHECK_THAT(graph.vertices(), CM::NoneMatch(WasReachedBy(dial)));

        const auto distances =
                graph.vertices() | ranges::views::transform([&](const auto& vertex) {
                    return dial.distance_to_vertex(vertex);
                });
        CATCH_CHECK_THAT(distances, ww::testing::AllEqualTo(max_distance));
    }

    CATCH_SECTION("{distance,graph,vertex,edge}_type")
    {
        CATCH_STATIC_REQUIRE((std::is_same_v<Distance_, Distance>));
        CATCH_STATIC_REQUIRE((std::is_same_v<Graph_, Graph>));
        CATCH_STATIC_REQUIRE((std::is_same_v<Vertex, Graph::vertex_type>));
        CATCH_STATIC_REQUIRE((std::is_same_v<Edge, Graph::edge_type>));
        CATCH_STATIC_REQUIRE((std::is_same_v<Size, std::size_t>));
    }

    CATCH_SECTION("get_bucket_id")
    {
        const auto source = 0U;
        dial.add_source(source);

        const auto edges = {0U, 1U, 2U};
        const auto heads = {1U, 2U, 3U};
        const auto lengths = {1, 10, 100};

        auto tail = source;
        auto total_distance = 0;
        for (auto&& [edge, head, length] : ranges::views::zip(edges, heads, lengths)) {
            dial.visit_vertex(tail, total_distance);
            total_distance += length;
            dial.relax_edge(edge, tail, head, total_distance);
            tail = head;
        }

        const auto vertices = {0U, 1U, 2U, 3U};
        const auto distances = {0, 1, 11, 111};
        for (auto&& [vertex, distance] : ranges::views::zip(vertices, distances)) {
            const auto bucket_id = dial.get_bucket_id(distance);
            CATCH_CHECK(bucket_id == static_cast<Size>(distance) % num_buckets);
            auto& bucket = dial.get_bucket(bucket_id);
            CATCH_CHECK(bucket.front() == vertex);
            bucket.pop();
        }

        CATCH_CHECK_THAT(dial.buckets(), CM::AllMatch(CM::IsEmpty()));
    }

    CATCH_SECTION("pop_next_unvisited_vertex")
    {
        const auto vertex0 = 0U;
        const auto distance0 = 0;
        dial.add_source(vertex0);
        CATCH_CHECK(dial.current_bucket_id() == 0U);
        CATCH_CHECK(std::size(dial.current_bucket()) == 1U);
        CATCH_CHECK(dial.current_bucket().front() == vertex0);

        const auto [vertex1, distance1] = dial.pop_next_unvisited_vertex();
        CATCH_CHECK(dial.current_bucket_id() == 0U);
        CATCH_CHECK_THAT(dial.current_bucket(), CM::IsEmpty());
        CATCH_CHECK(vertex1 == vertex0);
        CATCH_CHECK(distance1 == distance0);
    }

    CATCH_SECTION("add_source")
    {
        const auto sources = {0U, 1U};
        for (const auto& source : sources) {
            dial.add_source(source);
        }

        CATCH_CHECK(dial.current_bucket_id() == 0U);
        CATCH_CHECK(std::size(dial.current_bucket()) == std::size(sources));
        CATCH_CHECK_THAT(sources, CM::AllMatch(ww::testing::WasReachedBy(dial)));

        const auto [vertex0, distance0] = dial.pop_next_unvisited_vertex();
        CATCH_CHECK_THAT(sources, CM::Contains(vertex0));
        CATCH_CHECK(distance0 == 0);

        const auto [vertex1, distance1] = dial.pop_next_unvisited_vertex();
        CATCH_CHECK_THAT(sources, CM::Contains(vertex1));
        CATCH_CHECK(distance1 == 0);

        CATCH_CHECK(dial.current_bucket_id() == 0U);
        CATCH_CHECK_THAT(dial.current_bucket(), CM::IsEmpty());
        CATCH_CHECK(vertex0 != vertex1);
    }

    CATCH_SECTION("visit_vertex")
    {
        const auto vertex0 = 0U;
        const auto distance0 = 0;
        dial.add_source(vertex0);

        using ww::testing::WasVisitedBy;
        CATCH_CHECK_THAT(vertex0, !WasVisitedBy(dial));
        dial.visit_vertex(vertex0, distance0);
        CATCH_CHECK_THAT(vertex0, WasVisitedBy(dial));
        CATCH_CHECK(dial.distance_to_vertex(vertex0) == distance0);

        const auto edge = 0U;
        const auto vertex1 = 1U;
        const auto distance1 = 10;
        dial.relax_edge(edge, vertex0, vertex1, distance1);
        CATCH_CHECK_THAT(vertex1, !WasVisitedBy(dial));

        dial.visit_vertex(vertex1, distance1);
        CATCH_CHECK_THAT(vertex1, WasVisitedBy(dial));
        CATCH_CHECK(dial.distance_to_vertex(vertex1) == distance1);
    }

    CATCH_SECTION("relax_edge")
    {
        const auto source = 0U;
        dial.add_source(source);

        const auto [tail, distance] = dial.pop_next_unvisited_vertex();
        dial.visit_vertex(tail, distance);
        CATCH_CHECK(tail == source);
        CATCH_CHECK(distance == 0);

        const auto edge = 0U;
        const auto head = 1U;
        const auto length = 10;
        dial.relax_edge(edge, tail, head, distance + length);

        CATCH_CHECK_THAT(head, ww::testing::WasReachedBy(dial));
        CATCH_CHECK_THAT(head, !ww::testing::WasVisitedBy(dial));
        CATCH_CHECK(dial.distance_to_vertex(head) == distance + length);

        const auto bucket_id = dial.get_bucket_id(distance + length);
        const auto& bucket = dial.get_bucket(bucket_id);
        CATCH_CHECK(std::size(bucket) == 1U);
        CATCH_CHECK(bucket.front() == head);
    }

    CATCH_SECTION("done")
    {
        CATCH_CHECK(dial.done());
        dial.add_source(0U);
        CATCH_CHECK_FALSE(dial.done());
        dial.pop_next_unvisited_vertex();
        CATCH_CHECK(dial.done());
    }

    CATCH_SECTION("reset")
    {
        const auto source = 0U;
        dial.add_source(source);

        const auto edges = {0U, 1U, 2U};
        const auto heads = {1U, 2U, 3U};
        const auto lengths = {1, 10, 100};

        auto tail = source;
        auto total_distance = 0;
        for (auto&& [edge, head, length] : ranges::views::zip(edges, heads, lengths)) {
            dial.visit_vertex(tail, total_distance);
            total_distance += length;
            dial.relax_edge(edge, tail, head, total_distance);
            tail = head;
        }

        dial.reset();

        CATCH_CHECK_THAT(dial.buckets(), CM::AllMatch(CM::IsEmpty()));
        CATCH_CHECK(dial.current_bucket_id() == 0U);
        CATCH_CHECK(dial.done());

        using ww::testing::WasReachedBy;
        CATCH_CHECK_THAT(graph.vertices(), CM::NoneMatch(WasReachedBy(dial)));

        const auto distances =
                graph.vertices() | ranges::views::transform([&](const auto& vertex) {
                    return dial.distance_to_vertex(vertex);
                });
        CATCH_CHECK_THAT(distances, ww::testing::AllEqualTo(max_distance));
    }
}

CATCH_TEST_CASE("Dial (zero buckets)", "[graph]")
{
    const auto graph = ww::RectangularGridGraph<>(4U, 4U);
    const auto num_buckets = 0U;
    auto dial = ww::Dial<unsigned, decltype(graph)>(graph, num_buckets);

    CATCH_CHECK(dial.num_buckets() == 0U);
    CATCH_CHECK(dial.done());

    CATCH_CHECK(dial.current_bucket_id() == 0U);
    dial.advance_current_bucket();
    CATCH_CHECK(dial.current_bucket_id() == 0U);
}

CATCH_TEST_CASE("Dial.advance_current_bucket", "[graph]")
{
    const auto graph = ww::RectangularGridGraph<>(4U, 4U);
    const auto num_buckets = 2U;
    auto dial = ww::Dial<unsigned, decltype(graph)>(graph, num_buckets);

    CATCH_CHECK(dial.current_bucket_id() == 0U);
    dial.advance_current_bucket();
    CATCH_CHECK(dial.current_bucket_id() == 1U);
    dial.advance_current_bucket();
    CATCH_CHECK(dial.current_bucket_id() == 0U);
}

CATCH_TEST_CASE("Dial.done", "[graph]")
{
    using Distance = int;
    using Graph = ww::CSRGraph<>;

    const auto tail = 0U;
    const auto heads = {1U, 2U};
    const auto edges = {0U, 1U};
    const auto distances = {1, 10};

    auto edgelist = ww::EdgeList();
    for (const auto& head : heads) {
        edgelist.add_edge(tail, head);
    }

    const auto graph = Graph(edgelist);

    const auto num_buckets = 11U;
    auto dial = ww::Dial<Distance, Graph>(graph, num_buckets);

    dial.add_source(tail);
    dial.visit_vertex(tail, 0);
    for (auto&& [edge, head, length] : ranges::views::zip(edges, heads, distances)) {
        dial.relax_edge(edge, tail, head, length);
    }

    for (const auto& distance : distances) {
        const auto bucket_id = dial.get_bucket_id(distance);
        CATCH_CHECK_FALSE(dial.done());
        CATCH_CHECK(dial.current_bucket_id() == bucket_id);
        dial.pop_next_unvisited_vertex();
    }

    CATCH_CHECK(dial.done());
}

} // namespace
