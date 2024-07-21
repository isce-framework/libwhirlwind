#include <cmath>
#include <limits>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <whirlwind/math/numbers.hpp>

namespace {

namespace ww = whirlwind;

TEST_CASE("zero", "[numbers]")
{
    STATIC_REQUIRE(ww::zero<float>() == 0.0f);
    STATIC_REQUIRE(ww::zero<double>() == 0.0);
    STATIC_REQUIRE(ww::zero<int>() == 0);
    STATIC_REQUIRE(ww::zero<signed char>() == 0);
    STATIC_REQUIRE(ww::zero<unsigned long long>() == 0ULL);
}

TEST_CASE("one", "[numbers]")
{
    STATIC_REQUIRE(ww::one<float>() == 1.0f);
    STATIC_REQUIRE(ww::one<double>() == 1.0);
    STATIC_REQUIRE(ww::one<int>() == 1);
    STATIC_REQUIRE(ww::one<signed char>() == 1);
    STATIC_REQUIRE(ww::one<unsigned long long>() == 1ULL);
}

TEST_CASE("pi", "[numbers]")
{
    using Catch::Matchers::WithinAbs;
    CHECK_THAT(ww::pi<float>(), WithinAbs(3.141'592'7f, 1e-7));
    CHECK_THAT(ww::pi<double>(), WithinAbs(3.141'592'653'589'793'2, 1e-16));
}

TEST_CASE("pi (consteval)", "[numbers]")
{
    STATIC_REQUIRE(ww::pi<float>());
    STATIC_REQUIRE(ww::pi<double>());
}

TEST_CASE("tau", "[numbers]")
{
    using Catch::Matchers::WithinAbs;
    CHECK_THAT(ww::tau<float>(), WithinAbs(6.283'185'3f, 2e-7));
    CHECK_THAT(ww::tau<double>(), WithinAbs(6.283'185'307'179'586'4, 1e-16));
}

TEST_CASE("tau (consteval)", "[numbers]")
{
    STATIC_REQUIRE(ww::tau<float>());
    STATIC_REQUIRE(ww::tau<double>());
}

TEMPLATE_TEST_CASE("infinity (floating)", "[numbers]", float, double)
{
    using T = TestType;
    CHECK(std::isinf(ww::infinity<T>()));
    CHECK(ww::infinity<T>() > std::numeric_limits<T>::max());
    CHECK(-ww::infinity<T>() < std::numeric_limits<T>::lowest());
}

TEMPLATE_TEST_CASE(
        "infinity (integer)", "[numbers]", int, signed char, unsigned long long)
{
    using T = TestType;
    CHECK(ww::infinity<T>() == std::numeric_limits<T>::max());
}

TEST_CASE("infinity (consteval)", "[numbers]")
{
    STATIC_REQUIRE(ww::infinity<float>());
    STATIC_REQUIRE(ww::infinity<double>());
    STATIC_REQUIRE(ww::infinity<int>());
    STATIC_REQUIRE(ww::infinity<signed char>());
    STATIC_REQUIRE(ww::infinity<unsigned long long>());
}

} // namespace
