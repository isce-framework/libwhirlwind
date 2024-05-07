#pragma once

#ifdef MDSPAN_USE_BRACKET_OPERATOR
#undef MDSPAN_USE_BRACKET_OPERATOR
#endif
#define MDSPAN_USE_BRACKET_OPERATOR 1

#ifdef MDSPAN_USE_PAREN_OPERATOR
#undef MDSPAN_USE_PAREN_OPERATOR
#endif
#define MDSPAN_USE_PAREN_OPERATOR 1

#include <array>
#include <vector>

#include <experimental/mdarray>

#include "namespace.hpp"
#include "ndspan.hpp"
#include "stddef.hpp"

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

/**
 * A contiguous 1-dimensional array with static extent.
 *
 * @tparam T
 *     The element type.
 * @tparam N
 *     The array size.
 */
template<class T, Size N>
using StaticArray1D = NDArray<T, Extents<N>, std::array<T, N>>;

/**
 * A contiguous 2-dimensional array with static extent and row-major layout.
 *
 * @tparam T
 *     The element type.
 * @tparam M,N
 *     The array dimensions, in order from slowest varying to fastest varying.
 */
template<class T, Size M, Size N>
using StaticArray2D = NDArray<T, Extents<M, N>, std::array<T, M * N>>;

/**
 * A contiguous 3-dimensional array with static extent and row-major layout.
 *
 * @tparam T
 *     The element type.
 * @tparam M,N,P
 *     The array dimensions, in order from slowest varying to fastest varying.
 */
template<class T, Size M, Size N, Size P>
using StaticArray3D = NDArray<T, Extents<M, N, P>, std::array<T, M * N * P>>;

WHIRLWIND_NAMESPACE_END
