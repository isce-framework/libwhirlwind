#pragma once

#ifdef MDSPAN_USE_BRACKET_OPERATOR
#undef MDSPAN_USE_BRACKET_OPERATOR
#endif
#define MDSPAN_USE_BRACKET_OPERATOR 1

#ifdef MDSPAN_USE_PAREN_OPERATOR
#undef MDSPAN_USE_PAREN_OPERATOR
#endif
#define MDSPAN_USE_PAREN_OPERATOR 1

#include <cstddef>

#include <experimental/mdspan>

#include "namespace.hpp"

WHIRLWIND_NAMESPACE_BEGIN

/** A Fortran-style (column-major) array layout. */
using LayoutLeft = std::experimental::layout_left;

/** A C-style (row-major) array layout. */
using LayoutRight = std::experimental::layout_right;

/**
 * A multi-dimensional index space.
 *
 * `Extents` represents the dimensions of an array or span with fixed rank. Dimensions
 * may be dynamic or fixed at compile time (or a combination of the two).
 *
 * @tparam Dims
 *     The extent of each array dimension.
 */
template<std::size_t... Dims>
using Extents = std::experimental::extents<std::size_t, Dims...>;

template<std::size_t Rank>
using DynamicExtents = std::experimental::dextents<std::size_t, Rank>;

/** 1-dimensional dynamic extents. */
using DynamicExtents1D = DynamicExtents<1>;

/** 2-dimensional dynamic extents. */
using DynamicExtents2D = DynamicExtents<2>;

/** 3-dimensional dynamic extents. */
using DynamicExtents3D = DynamicExtents<3>;

template<class T, class Extents, class LayoutPolicy = LayoutRight>
using NDSpan = std::experimental::mdspan<T, Extents, LayoutPolicy>;

/**
 * A non-owning 1-dimensional view of an array of elements with dynamic extent.
 *
 * @tparam T
 *     The element type.
 * @tparam LayoutPolicy
 *     Specifies the layout of the array in memory.
 */
template<class T, class LayoutPolicy = LayoutRight>
using Span1D = NDSpan<T, DynamicExtents1D, LayoutPolicy>;

/**
 * A non-owning 2-dimensional view of an array of elements with dynamic extent.
 *
 * @tparam T
 *     The element type.
 * @tparam LayoutPolicy
 *     Specifies the layout of the array in memory.
 */
template<class T, class LayoutPolicy = LayoutRight>
using Span2D = NDSpan<T, DynamicExtents2D, LayoutPolicy>;

/**
 * A non-owning 3-dimensional view of an array of elements with dynamic extent.
 *
 * @tparam T
 *     The element type.
 * @tparam LayoutPolicy
 *     Specifies the layout of the array in memory.
 */
template<class T, class LayoutPolicy = LayoutRight>
using Span3D = NDSpan<T, DynamicExtents3D, LayoutPolicy>;

/** A constant used to differentiate arrays/spans of static and dynamic extent. */
inline constexpr std::size_t dynamic = std::experimental::dynamic_extent;

WHIRLWIND_NAMESPACE_END
