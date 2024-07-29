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

TEST_CASE("ForestType", "[graph]")
{
    using Graph = ww::CSRGraph<>;
    using Distance = int;
    require_satisfies_forest_type<ww::Forest<Graph>>();
    require_satisfies_forest_type<ww::ShortestPathForest<Distance, Graph>>();
}

TEST_CASE("MutableForestType", "[graph]")
{
    using Graph = ww::CSRGraph<>;
    using Distance = int;
    require_satisfies_mutable_forest_type<ww::Forest<Graph>>();
    require_satisfies_mutable_forest_type<ww::ShortestPathForest<Distance, Graph>>();
}

} // namespace
