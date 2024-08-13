#include <cmath>
#include <limits>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <whirlwind/math/numbers.hpp>

namespace {

namespace CM = Catch::Matchers;
namespace ww = whirlwind;

CATCH_TEST_CASE("zero", "[numbers]")
{
    CATCH_STATIC_REQUIRE(ww::zero<float>() == 0.0F);
    CATCH_STATIC_REQUIRE(ww::zero<double>() == 0.0);
    CATCH_STATIC_REQUIRE(ww::zero<int>() == 0);
    CATCH_STATIC_REQUIRE(ww::zero<signed char>() == 0);
    CATCH_STATIC_REQUIRE(ww::zero<unsigned long long>() == 0ULL);
}

CATCH_TEST_CASE("one", "[numbers]")
{
    CATCH_STATIC_REQUIRE(ww::one<float>() == 1.0F);
    CATCH_STATIC_REQUIRE(ww::one<double>() == 1.0);
    CATCH_STATIC_REQUIRE(ww::one<int>() == 1);
    CATCH_STATIC_REQUIRE(ww::one<signed char>() == 1);
    CATCH_STATIC_REQUIRE(ww::one<unsigned long long>() == 1ULL);
}

CATCH_TEST_CASE("pi", "[numbers]")
{
    CATCH_CHECK_THAT(ww::pi<float>(), CM::WithinAbs(3.141'592'7, 1e-7));
    CATCH_CHECK_THAT(ww::pi<double>(), CM::WithinAbs(3.141'592'653'589'793'2, 1e-16));
}

CATCH_TEST_CASE("pi (consteval)", "[numbers]")
{
    CATCH_STATIC_REQUIRE(ww::pi<float>());
    CATCH_STATIC_REQUIRE(ww::pi<double>());
}

CATCH_TEST_CASE("tau", "[numbers]")
{
    CATCH_CHECK_THAT(ww::tau<float>(), CM::WithinAbs(6.283'185'3, 2e-7));
    CATCH_CHECK_THAT(ww::tau<double>(), CM::WithinAbs(6.283'185'307'179'586'4, 1e-16));
}

CATCH_TEST_CASE("tau (consteval)", "[numbers]")
{
    CATCH_STATIC_REQUIRE(ww::tau<float>());
    CATCH_STATIC_REQUIRE(ww::tau<double>());
}

CATCH_TEMPLATE_TEST_CASE("infinity (floating)", "[numbers]", float, double)
{
    using T = TestType;
    CATCH_CHECK(std::isinf(ww::infinity<T>()));
    CATCH_CHECK(ww::infinity<T>() > std::numeric_limits<T>::max());
    CATCH_CHECK(-ww::infinity<T>() < std::numeric_limits<T>::lowest());
}

CATCH_TEMPLATE_TEST_CASE(
        "infinity (integer)", "[numbers]", int, signed char, unsigned long long)
{
    using T = TestType;
    CATCH_CHECK(ww::infinity<T>() == std::numeric_limits<T>::max());
}

CATCH_TEST_CASE("infinity (consteval)", "[numbers]")
{
    CATCH_STATIC_REQUIRE(ww::infinity<float>());
    CATCH_STATIC_REQUIRE(ww::infinity<double>());
    CATCH_STATIC_REQUIRE(ww::infinity<int>());
    CATCH_STATIC_REQUIRE(ww::infinity<signed char>());
    CATCH_STATIC_REQUIRE(ww::infinity<unsigned long long>());
}

} // namespace
