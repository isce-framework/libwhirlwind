#include <catch2/catch_test_macros.hpp>

#include <whirlwind/common/compatibility.hpp>
#include <whirlwind/graph/csr_graph.hpp>
#include <whirlwind/graph/graph_concepts.hpp>
#include <whirlwind/graph/rectangular_grid_graph.hpp>

namespace {

namespace ww = whirlwind;

template<ww::GraphType Graph>
WHIRLWIND_CONSTEVAL void
require_satisfies_graph_type() noexcept
{}

TEST_CASE("GraphType", "[graph]")
{
    require_satisfies_graph_type<ww::CSRGraph<>>();
    require_satisfies_graph_type<ww::RectangularGridGraph<>>();
}

} // namespace
