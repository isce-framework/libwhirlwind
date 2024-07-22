#include <catch2/catch_test_macros.hpp>

#include <whirlwind/common/compatibility.hpp>
#include <whirlwind/graph/csr_graph.hpp>
#include <whirlwind/graph/forest.hpp>
#include <whirlwind/graph/shortest_path_forest.hpp>
#include <whirlwind/graph/simple_forest.hpp>

namespace {

namespace ww = whirlwind;

template<ww::Forest Forest>
WHIRLWIND_CONSTEVAL void
require_satisfies_forest() noexcept
{}

template<ww::MutableForest Forest>
WHIRLWIND_CONSTEVAL void
require_satisfies_mutable_forest() noexcept
{}

TEST_CASE("Forest", "[graph]")
{
    using Graph = ww::CSRGraph<>;
    using Distance = int;
    require_satisfies_forest<ww::SimpleForest<Graph>>();
    require_satisfies_forest<ww::ShortestPathForest<Distance, Graph>>();
}

TEST_CASE("MutableForest", "[graph]")
{
    using Graph = ww::CSRGraph<>;
    using Distance = int;
    require_satisfies_mutable_forest<ww::SimpleForest<Graph>>();
    require_satisfies_mutable_forest<ww::ShortestPathForest<Distance, Graph>>();
}

} // namespace
