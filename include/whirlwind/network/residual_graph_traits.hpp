#pragma once

#include <whirlwind/common/namespace.hpp>
#include <whirlwind/common/stddef.hpp>
#include <whirlwind/graph/csr_graph.hpp>
#include <whirlwind/graph/rectangular_grid_graph.hpp>

WHIRLWIND_NAMESPACE_BEGIN

template<class Graph>
struct ResidualGraphTraits;

template<template<class> class Container>
struct ResidualGraphTraits<CSRGraph<Container>> {
    using type = CSRGraph<Container>;
};

template<Size P, class Dim>
struct ResidualGraphTraits<RectangularGridGraph<P, Dim>> {
    using type = RectangularGridGraph<2 * P, Dim>;
};

WHIRLWIND_NAMESPACE_END
