#pragma once

#include <array>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>

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
class CubicBSpline3D {
public:
    using knot_type = Knot;
    using value_type = Value;
    using basis_type = Basis;
    using bases_type = std::array<Basis, 3>;
    using control_points_type = Array3D<Value, Container<Value>>;
    using size_type = std::size_t;

    template<class T>
    using container_type = Container<T>;

    constexpr CubicBSpline3D(bases_type bases, control_points_type control_points)
        : bases_(std::move(bases)), control_points_(std::move(control_points))
    {
        WHIRLWIND_ASSERT(control_points_.extent(0) == bases_[0].num_basis_funcs());
        WHIRLWIND_ASSERT(control_points_.extent(1) == bases_[1].num_basis_funcs());
        WHIRLWIND_ASSERT(control_points_.extent(2) == bases_[2].num_basis_funcs());
    }

    constexpr CubicBSpline3D(basis_type basis0,
                             basis_type basis1,
                             basis_type basis2,
                             control_points_type control_points)
        : CubicBSpline3D(
                  bases_type{std::move(basis0), std::move(basis1), std::move(basis2)},
                  std::move(control_points))
    {}

    template<class Tuple>
    constexpr CubicBSpline3D(Tuple&& bases, control_points_type control_points)
        : CubicBSpline3D(std::get<0>(bases),
                         std::get<1>(bases),
                         std::get<2>(bases),
                         std::move(control_points))
    {
        WHIRLWIND_STATIC_ASSERT(std::tuple_size_v<std::remove_cvref_t<Tuple>> == 3);
    }

    template<class InputRange>
    constexpr CubicBSpline3D(bases_type bases, const InputRange& control_points)
        : bases_(std::move(bases)), control_points_([&]() {
              auto c = ranges::to<container_type<value_type>>(control_points);
              const auto ext = DynamicExtents3D(bases_[0].num_basis_funcs(),
                                                bases_[1].num_basis_funcs(),
                                                bases_[2].num_basis_funcs());
              WHIRLWIND_ASSERT(std::size(c) ==
                               ext.extent(0) * ext.extent(1) * ext.extent(2));
              return control_points_type(std::move(c), ext);
          }())
    {}

    template<class InputRange>
    constexpr CubicBSpline3D(basis_type basis0,
                             basis_type basis1,
                             basis_type basis2,
                             const InputRange& control_points)
        : CubicBSpline3D(
                  bases_type{std::move(basis0), std::move(basis1), std::move(basis2)},
                  control_points)
    {}

    template<class Tuple, class InputRange>
    constexpr CubicBSpline3D(Tuple&& bases, const InputRange& control_points)
        : CubicBSpline3D(std::get<0>(bases),
                         std::get<1>(bases),
                         std::get<2>(bases),
                         control_points)
    {
        WHIRLWIND_STATIC_ASSERT(std::tuple_size_v<std::remove_cvref_t<Tuple>> == 3);
    }

    [[nodiscard]] constexpr auto
    operator()(const knot_type& x0,
               const knot_type& x1,
               const knot_type& x2) const -> value_type
    {
        const auto i0 = bases_[0].get_knot_interval(x0);
        const auto i1 = bases_[1].get_knot_interval(x1);
        const auto i2 = bases_[2].get_knot_interval(x2);

        const auto b0 = bases_[0].eval_in_interval(x0, i0);
        const auto b1 = bases_[1].eval_in_interval(x1, i1);
        const auto b2 = bases_[2].eval_in_interval(x2, i2);

        auto c2 = [&](size_type ii, size_type jj, size_type kk) noexcept {
            return control_points_(i0 + ii, i1 + jj, i2 + kk);
        };

        auto c1 = [&](size_type ii, size_type jj) noexcept {
            return (c2(ii, jj, 0) * b2[0] + c2(ii, jj, 1) * b2[1]) +
                   (c2(ii, jj, 2) * b2[2] + c2(ii, jj, 3) * b2[3]);
        };

        auto c0 = [&](size_type ii) noexcept {
            return (c1(ii, 0) * b1[0] + c1(ii, 1) * b1[1]) +
                   (c1(ii, 2) * b1[2] + c1(ii, 3) * b1[3]);
        };

        return (c0(0) * b0[0] + c0(1) * b0[1]) + (c0(2) * b0[2] + c0(3) * b0[3]);
    }

    template<class InputRange>
    [[nodiscard]] constexpr auto
    operator()(const InputRange& x0,
               const InputRange& x1,
               const InputRange& x2) const -> container_type<value_type>
    {
        return ranges::views::zip(x0, x1, x2) |
               ranges::views::transform([&](const auto& xx) {
                   using std::get;
                   return operator()(get<0>(xx), get<1>(xx), get<2>(xx));
               }) |
               ranges::to<container_type<value_type>>();
    }

    [[nodiscard]] static WHIRLWIND_CONSTEVAL auto
    num_dims() -> size_type
    {
        return size_type{3};
    }

    [[nodiscard]] constexpr auto
    knots(size_type i) const
    {
        WHIRLWIND_ASSERT(i < num_dims());
        return bases_[i].knots();
    }

    [[nodiscard]] constexpr auto
    control_points() const -> const control_points_type&
    {
        return control_points_;
    }

protected:
    bases_type bases_;
    control_points_type control_points_;
};

template<class Knot,
         class Value = Knot,
         template<class> class Container = Vector,
         class Basis = CubicBSplineBasis<Knot, Container>>
using TriCubicBSpline = CubicBSpline3D<Knot, Value, Container, Basis>;

WHIRLWIND_NAMESPACE_END
