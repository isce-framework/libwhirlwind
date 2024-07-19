#pragma once

#include <array>
#include <cmath>
#include <iterator>
#include <memory>
#include <span>
#include <type_traits>

#include <range/v3/algorithm/adjacent_find.hpp>
#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/algorithm/lower_bound.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/range/access.hpp>

#include <whirlwind/array/ndarray.hpp>
#include <whirlwind/common/assert.hpp>
#include <whirlwind/common/namespace.hpp>
#include <whirlwind/common/stddef.hpp>
#include <whirlwind/container/vector.hpp>

WHIRLWIND_NAMESPACE_BEGIN

template<class ForwardRange>
[[nodiscard]] constexpr auto
is_monotonically_increasing(ForwardRange&& r) -> bool
{
    return ranges::adjacent_find(r, ranges::greater{}) == ranges::end(r);
}

template<class Range>
[[nodiscard]] constexpr auto
is_contiguous_range(Range&& r) -> bool
{
    auto begin = ranges::begin(r);
    auto end = ranges::end(r);
    return ((std::to_address(begin) == std::addressof(*begin)) &&
            (std::to_address(end) == std::to_address(begin) + (end - begin)));
}

// TODO: concepts
template<class T>
[[nodiscard]] constexpr auto
is_contiguous_range(const Vector<T>&) noexcept -> bool
{
    return true;
}

template<class Knot, template<class> class Container = Vector>
class CubicBSplineBasis {
protected:
    template<class RandomAccessRange>
    [[nodiscard]] static constexpr auto
    make_augmented_knot_sequence(const RandomAccessRange& knots)
    {
        if constexpr (std::is_floating_point_v<knot_type>) {
            WHIRLWIND_ASSERT(ranges::all_of(
                    knots, [](const auto& x) { return !std::isnan(x); }));
        }
        WHIRLWIND_ASSERT(is_monotonically_increasing(knots));

        const auto n = std::size(knots);
        WHIRLWIND_ASSERT(n >= 2);

        const auto t0 = knots[0];
        const auto t1 = knots[1];
        const auto tn1 = knots[n - 1];
        const auto tn2 = knots[n - 2];

        const auto dt0 = t1 - t0;
        const auto dtn = tn1 - tn2;

        auto augmented_knots = Container<Knot>(n + 4);

        augmented_knots[0] = t0 - dt0 - dt0;
        augmented_knots[1] = t0 - dt0;

        using Index = std::remove_const_t<decltype(n)>;
        for (Index i = 0; i != n; ++i) {
            augmented_knots[i + 2] = knots[i];
        }

        augmented_knots[n + 2] = tn1 + dtn;
        augmented_knots[n + 3] = tn1 + dtn + dtn;

        return augmented_knots;
    }

    template<class RandomAccessRange>
    [[nodiscard]] static constexpr auto
    precompute_de_boor_basis_coeffs(const RandomAccessRange& augmented_knots)
    {
        const auto n = std::size(augmented_knots);
        WHIRLWIND_ASSERT(n >= 6);
        const auto num_intervals = n - 5;

        auto de_boor_coeffs =
                NDArray<Knot, Extents<dynamic, 4>, Container<Knot>>(num_intervals, 4);

        auto safe_divide = [](const Knot& x1, const Knot& x2) -> Knot {
            return (x2 == 0) ? 0 : x1 / x2;
        };

        using Index = std::remove_const_t<decltype(num_intervals)>;
        for (Index i = 0; i != num_intervals; ++i) {
            const auto t0 = augmented_knots[i];
            const auto t1 = augmented_knots[i + 1];
            const auto t2 = augmented_knots[i + 2];
            const auto t3 = augmented_knots[i + 3];
            const auto t4 = augmented_knots[i + 4];
            const auto t5 = augmented_knots[i + 5];

            const auto dt30 = t3 - t0;
            const auto dt31 = t3 - t1;
            const auto dt32 = t3 - t2;
            const auto dt41 = t4 - t1;
            const auto dt42 = t4 - t2;
            const auto dt52 = t5 - t2;

            de_boor_coeffs(i, 0) = safe_divide(1, dt52 * dt42 * dt32);
            de_boor_coeffs(i, 1) = safe_divide(1, dt41 * dt31 * dt32);
            de_boor_coeffs(i, 2) = safe_divide(1, dt41 * dt42 * dt32);
            de_boor_coeffs(i, 3) = safe_divide(1, dt30 * dt31 * dt32);
        }

        return de_boor_coeffs;
    }

public:
    using knot_type = Knot;
    using size_type = Size;

    template<class T>
    using container_type = Container<T>;

    template<class RandomAccessRange>
    explicit constexpr CubicBSplineBasis(const RandomAccessRange& knots)
        : augmented_knots_(make_augmented_knot_sequence(knots)),
          de_boor_coeffs_(precompute_de_boor_basis_coeffs(augmented_knots_))
    {}

    [[nodiscard]] constexpr auto
    knots() const
    {
        WHIRLWIND_ASSERT(is_contiguous_range(augmented_knots_));
        WHIRLWIND_DEBUG_ASSERT(std::size(augmented_knots_) >= 4);
        const auto first = std::to_address(ranges::begin(augmented_knots_) + 2);
        const auto count = std::size(augmented_knots_) - 4;
        return std::span(first, count);
    }

    [[nodiscard]] constexpr auto
    num_knot_intervals() const noexcept -> size_type
    {
        WHIRLWIND_DEBUG_ASSERT(std::size(augmented_knots_) >= 5);
        return std::size(augmented_knots_) - 5;
    }

    [[nodiscard]] constexpr auto
    num_basis_funcs() const noexcept -> size_type
    {
        WHIRLWIND_DEBUG_ASSERT(std::size(augmented_knots_) >= 2);
        return std::size(augmented_knots_) - 2;
    }

    [[nodiscard]] constexpr auto
    get_knot_interval(const knot_type& x) const -> size_type
    {
        WHIRLWIND_ASSERT(!std::isnan(x));
        WHIRLWIND_ASSERT(is_contiguous_range(augmented_knots_));

        WHIRLWIND_DEBUG_ASSERT(std::size(augmented_knots_) >= 6);
        const auto first = std::to_address(ranges::begin(augmented_knots_) + 3);
        const auto count = std::size(augmented_knots_) - 6;
        const auto subspan = std::span(first, count);

        const auto it = ranges::lower_bound(subspan, x);
        const auto i = size_type{std::distance(first, std::to_address(it))};
        WHIRLWIND_DEBUG_ASSERT(i < num_knot_intervals());
        return i;
    }

    [[nodiscard]] constexpr auto
    eval_in_interval(const knot_type& x, size_type i) const
    {
        WHIRLWIND_ASSERT(i < num_knot_intervals());

        WHIRLWIND_DEBUG_ASSERT(i < de_boor_coeffs_.extent(0));
        WHIRLWIND_DEBUG_ASSERT(de_boor_coeffs_.extent(1) >= 4);
        const auto c0 = de_boor_coeffs_(i, 0);
        const auto c1 = de_boor_coeffs_(i, 1);
        const auto c2 = de_boor_coeffs_(i, 2);
        const auto c3 = de_boor_coeffs_(i, 3);

        WHIRLWIND_DEBUG_ASSERT(i + 5 < std::size(augmented_knots_));
        const auto dt5x = augmented_knots_[i + 5] - x;
        const auto dt4x = augmented_knots_[i + 4] - x;
        const auto dt3x = augmented_knots_[i + 3] - x;
        WHIRLWIND_DEBUG_ASSERT(dt5x >= 0);
        WHIRLWIND_DEBUG_ASSERT(dt4x >= 0);
        WHIRLWIND_DEBUG_ASSERT(dt3x >= 0);

        const auto dxt2 = x - augmented_knots_[i + 2];
        const auto dxt1 = x - augmented_knots_[i + 1];
        const auto dxt0 = x - augmented_knots_[i];
        WHIRLWIND_DEBUG_ASSERT(dxt2 >= 0);
        WHIRLWIND_DEBUG_ASSERT(dxt1 >= 0);
        WHIRLWIND_DEBUG_ASSERT(dxt0 >= 0);

        const auto y3 = c0 * (dxt2 * dxt2 * dxt2);
        const auto y2 = c0 * (dt5x * dxt2 * dxt2) + c1 * (dxt1 * dxt1 * dt3x) +
                        c2 * (dxt1 * dt4x * dxt2);
        const auto y1 = c1 * (dt4x * dxt1 * dt3x) + c2 * (dt4x * dt4x * dxt2) +
                        c3 * (dxt0 * dt3x * dt3x);
        const auto y0 = c3 * (dt3x * dt3x * dt3x);
        WHIRLWIND_DEBUG_ASSERT(y0 >= 0);
        WHIRLWIND_DEBUG_ASSERT(y1 >= 0);
        WHIRLWIND_DEBUG_ASSERT(y2 >= 0);
        WHIRLWIND_DEBUG_ASSERT(y3 >= 0);

        return std::array{y0, y1, y2, y3};
    }

    [[nodiscard]] constexpr auto
    eval_derivative_in_interval(const knot_type& x, size_type i) const
    {
        WHIRLWIND_ASSERT(i < num_knot_intervals());

        WHIRLWIND_DEBUG_ASSERT(i < de_boor_coeffs_.extent(0));
        WHIRLWIND_DEBUG_ASSERT(de_boor_coeffs_.extent(1) >= 4);
        const auto c0 = de_boor_coeffs_(i, 0);
        const auto c1 = de_boor_coeffs_(i, 1);
        const auto c2 = de_boor_coeffs_(i, 2);
        const auto c3 = de_boor_coeffs_(i, 3);

        WHIRLWIND_DEBUG_ASSERT(i + 5 < std::size(augmented_knots_));
        const auto dt5x = augmented_knots_[i + 5] - x;
        const auto dt4x = augmented_knots_[i + 4] - x;
        const auto dt3x = augmented_knots_[i + 3] - x;
        WHIRLWIND_DEBUG_ASSERT(dt5x >= 0);
        WHIRLWIND_DEBUG_ASSERT(dt4x >= 0);
        WHIRLWIND_DEBUG_ASSERT(dt3x >= 0);

        const auto dxt2 = x - augmented_knots_[i + 2];
        const auto dxt1 = x - augmented_knots_[i + 1];
        const auto dxt0 = x - augmented_knots_[i];
        WHIRLWIND_DEBUG_ASSERT(dxt2 >= 0);
        WHIRLWIND_DEBUG_ASSERT(dxt1 >= 0);
        WHIRLWIND_DEBUG_ASSERT(dxt0 >= 0);

        const auto y3 = knot_type{3} * c0 * dxt2 * dxt2;
        const auto y2 = c0 * dxt2 * (knot_type{2} * dt5x - dxt2) +
                        c1 * dxt1 * (knot_type{2} * dt3x - dxt1) +
                        c2 * (dt4x * dxt2 + dxt1 * dt4x - dxt1 * dxt2);
        const auto y1 = c1 * (dt4x * dt3x - dxt1 * dt3x - dt4x * dxt1) +
                        c2 * dt4x * (dt4x - knot_type{2} * dxt2) +
                        c3 * dt3x * (dt3x - knot_type{2} * dxt0);
        const auto y0 = knot_type{-3} * c3 * dt3x * dt3x;

        return std::array{y0, y1, y2, y3};
    }

    [[nodiscard]] constexpr auto
    eval_second_derivative_in_interval(const knot_type& x, size_type i) const
    {
        WHIRLWIND_ASSERT(i < num_knot_intervals());

        WHIRLWIND_DEBUG_ASSERT(i < de_boor_coeffs_.extent(0));
        WHIRLWIND_DEBUG_ASSERT(de_boor_coeffs_.extent(1) >= 4);
        const auto c0 = de_boor_coeffs_(i, 0);
        const auto c1 = de_boor_coeffs_(i, 1);
        const auto c2 = de_boor_coeffs_(i, 2);
        const auto c3 = de_boor_coeffs_(i, 3);

        WHIRLWIND_DEBUG_ASSERT(i + 5 < std::size(augmented_knots_));
        const auto dt5x = augmented_knots_[i + 5] - x;
        const auto dt4x = augmented_knots_[i + 4] - x;
        const auto dt3x = augmented_knots_[i + 3] - x;
        WHIRLWIND_DEBUG_ASSERT(dt5x >= 0);
        WHIRLWIND_DEBUG_ASSERT(dt4x >= 0);
        WHIRLWIND_DEBUG_ASSERT(dt3x >= 0);

        const auto dxt2 = x - augmented_knots_[i + 2];
        const auto dxt1 = x - augmented_knots_[i + 1];
        const auto dxt0 = x - augmented_knots_[i];
        WHIRLWIND_DEBUG_ASSERT(dxt2 >= 0);
        WHIRLWIND_DEBUG_ASSERT(dxt1 >= 0);
        WHIRLWIND_DEBUG_ASSERT(dxt0 >= 0);

        const auto y3 = knot_type{6} * c0 * dxt2;
        const auto y2 = knot_type{2} *
                        (c0 * (dt5x - knot_type{2} * dxt2) +
                         c1 * (dt3x - knot_type{2} * dxt1) + c2 * (dt4x - dxt2 - dxt1));
        const auto y1 = knot_type{2} *
                        (c1 * (dxt1 - dt3x - dt4x) + c2 * (dxt2 - knot_type{2} * dt4x) +
                         c3 * (dxt0 - knot_type{2} * dt3x));
        const auto y0 = knot_type{6} * c3 * dt3x;

        return std::array{y0, y1, y2, y3};
    }

private:
    container_type<knot_type> augmented_knots_;
    NDArray<knot_type, Extents<dynamic, 4>, container_type<knot_type>> de_boor_coeffs_;
};

WHIRLWIND_NAMESPACE_END
