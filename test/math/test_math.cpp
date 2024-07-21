#include <catch2/catch_test_macros.hpp>

#include <whirlwind/math/math.hpp>

namespace {

namespace ww = whirlwind;

TEST_CASE("is_even", "[math]")
{
    STATIC_REQUIRE(ww::is_even(0));
    STATIC_REQUIRE(ww::is_even(-0));
    STATIC_REQUIRE(ww::is_even(2));
    STATIC_REQUIRE(ww::is_even(-1'000'000'000LL));
    STATIC_REQUIRE(!ww::is_even(1));
    STATIC_REQUIRE(!ww::is_even(-1));
    STATIC_REQUIRE(!ww::is_even(1'000'000'001LL));
}

} // namespace
