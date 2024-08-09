#include <catch2/catch_test_macros.hpp>

#include <whirlwind/common/compatibility.hpp>
#include <whirlwind/graph/csr_graph.hpp>
#include <whirlwind/graph/dial.hpp>
#include <whirlwind/graph/dijkstra.hpp>
#include <whirlwind/graph/dijkstra_concepts.hpp>

namespace {

namespace ww = whirlwind;

template<ww::DijkstraSolverType DijkstraSolver>
WHIRLWIND_CONSTEVAL void
require_satisfies_dijkstra_solver_type() noexcept
{}

CATCH_TEST_CASE("DijkstraSolverType", "[graph]")
{
    using Distance = int;
    using Graph = ww::CSRGraph<>;
    require_satisfies_dijkstra_solver_type<ww::Dijkstra<Distance, Graph>>();
    require_satisfies_dijkstra_solver_type<ww::Dial<Distance, Graph>>();
}

} // namespace
