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
#include <vector>

#include <experimental/mdarray>

#include "namespace.hpp"
#include "ndspan.hpp"

WHIRLWIND_NAMESPACE_BEGIN

/**
 * A contiguous C-style (row-major) N-dimensional array.
 *
 * @tparam T
 *     The element type.
 * @tparam Extents
 *     The array dimensions.
 * @tparam Container
 *     The underlying container type.
 */
template<class T, class Extents, class Container = std::vector<T>>
using NDArray = std::experimental::mdarray<T, Extents, LayoutRight, Container>;

/**
 * @brief A contiguous 1-dimensional array with dynamic extent.
 *
 * @tparam T
 *     The element type.
 * @tparam Container
 *     The underlying container type.
 */
template<class T, class Container = std::vector<T>>
using Array1D = NDArray<T, DynamicExtents1D, Container>;

/**
 * @brief A contiguous 2-dimensional array with dynamic extent and row-major layout.
 *
 * @tparam T
 *     The element type.
 * @tparam Container
 *     The underlying container type.
 */
template<class T, class Container = std::vector<T>>
using Array2D = NDArray<T, DynamicExtents2D, Container>;

/**
 * @brief A contiguous 3-dimensional array with dynamic extent and row-major layout.
 *
 * @tparam T
 *     The element type.
 * @tparam Container
 *     The underlying container type.
 */
template<class T, class Container = std::vector<T>>
using Array3D = NDArray<T, DynamicExtents3D, Container>;

WHIRLWIND_NAMESPACE_END
