#pragma once

#include <cstddef>
#include <utility>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>

#include <whirlwind/common/assert.hpp>
#include <whirlwind/common/compatibility.hpp>
#include <whirlwind/common/namespace.hpp>
#include <whirlwind/container/vector.hpp>
#include <whirlwind/ndarray/ndarray.hpp>

#include "cubic_b_spline_basis.hpp"

WHIRLWIND_NAMESPACE_BEGIN

template<class Knot,
         class Value = Knot,
         template<class> class Container = Vector,
         class Basis = CubicBSplineBasis<Knot, Container>>
class CubicBSpline {
public:
    using knot_type = Knot;
    using value_type = Value;
    using basis_type = Basis;
    using control_points_type = Array1D<Value, Container<Value>>;
    using size_type = std::size_t;

    template<class T>
    using container_type = Container<T>;

    constexpr CubicBSpline(basis_type basis, control_points_type control_points)
        : basis_(std::move(basis)), control_points_(std::move(control_points))
    {
        WHIRLWIND_ASSERT(std::size(control_points_) == basis_.num_basis_funcs());
    }

    template<class InputRange>
    constexpr CubicBSpline(basis_type basis, const InputRange& control_points)
        : basis_(std::move(basis)), control_points_([&]() {
              auto c = ranges::to<container_type<value_type>>(control_points);
              const auto ext = DynamicExtents1D(basis_.num_basis_funcs());
              WHIRLWIND_ASSERT(std::size(c) == ext.extent(0));
              return control_points_type(std::move(c), ext);
          }())
    {}

    [[nodiscard]] constexpr auto
    operator()(const knot_type& x) const -> value_type
    {
        const auto i = basis_.get_knot_interval(x);
        const auto b = basis_.eval_in_interval(x, i);

        auto c = [&](size_type ii) noexcept { return control_points_[i + ii]; };

        return (c(0) * b[0] + c(1) * b[1]) + (c(2) * b[2] + c(3) * b[3]);
    }

    template<class InputRange>
    [[nodiscard]] constexpr auto
    operator()(const InputRange& x) const -> container_type<value_type>
    {
        return x | ranges::views::transform([&](const auto& xx) {
                   return operator()(xx);
               }) |
               ranges::to<container_type<value_type>>();
    }

    [[nodiscard]] static WHIRLWIND_CONSTEVAL auto
    num_dims() -> size_type
    {
        return size_type{1};
    }

    [[nodiscard]] constexpr auto
    knots() const
    {
        return basis_.knots();
    }

    [[nodiscard]] constexpr auto
    control_points() const noexcept -> const control_points_type&
    {
        return control_points_;
    }

protected:
    basis_type basis_;
    control_points_type control_points_;
};

WHIRLWIND_NAMESPACE_END
