#include <catch2/catch_test_macros.hpp>

#include <whirlwind/common/compatibility.hpp>
#include <whirlwind/graph/csr_graph.hpp>
#include <whirlwind/graph/digraph.hpp>
#include <whirlwind/graph/rectangular_grid_graph.hpp>

namespace {

namespace ww = whirlwind;

template<ww::DiGraph Graph>
WHIRLWIND_CONSTEVAL void
require_satisfies_digraph() noexcept
{}

TEST_CASE("digraph", "[graph]")
{
    require_satisfies_digraph<ww::CSRGraph<>>();
    require_satisfies_digraph<ww::RectangularGridGraph<>>();
}

} // namespace
