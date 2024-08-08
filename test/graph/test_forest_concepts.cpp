#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <whirlwind/common/compatibility.hpp>
#include <whirlwind/graph/csr_graph.hpp>
#include <whirlwind/graph/forest.hpp>
#include <whirlwind/graph/forest_concepts.hpp>
#include <whirlwind/graph/shortest_path_forest.hpp>

namespace {

namespace ww = whirlwind;

template<ww::ForestType Forest>
WHIRLWIND_CONSTEVAL void
require_satisfies_forest_type() noexcept
{}

template<ww::MutableForestType Forest>
WHIRLWIND_CONSTEVAL void
require_satisfies_mutable_forest_type() noexcept
{}

template<ww::ShortestPathForestType Forest>
WHIRLWIND_CONSTEVAL void
require_satisfies_shortest_path_forest_type() noexcept
{}

template<ww::MutableShortestPathForestType Forest>
WHIRLWIND_CONSTEVAL void
require_satisfies_mutable_shortest_path_forest_type() noexcept
{}

CATCH_TEST_CASE("ForestType", "[graph]")
{
    using Graph = ww::CSRGraph<>;
    using Distance = int;
    require_satisfies_forest_type<ww::Forest<Graph>>();
    require_satisfies_forest_type<ww::ShortestPathForest<Distance, Graph>>();
}

CATCH_TEST_CASE("MutableForestType", "[graph]")
{
    using Graph = ww::CSRGraph<>;
    using Distance = int;
    require_satisfies_mutable_forest_type<ww::Forest<Graph>>();
    require_satisfies_mutable_forest_type<ww::ShortestPathForest<Distance, Graph>>();
}

CATCH_TEMPLATE_TEST_CASE("ShortestPathForestType", "[graph]", int, float)
{
    using Distance = TestType;
    using Graph = ww::CSRGraph<>;
    using Forest = ww::ShortestPathForest<Distance, Graph>;
    require_satisfies_shortest_path_forest_type<Forest>();
}

CATCH_TEMPLATE_TEST_CASE("MutableShortestPathForestType", "[graph]", int, float)
{
    using Distance = TestType;
    using Graph = ww::CSRGraph<>;
    using Forest = ww::ShortestPathForest<Distance, Graph>;
    require_satisfies_mutable_shortest_path_forest_type<Forest>();
}

} // namespace
