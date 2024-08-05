#include <catch2/catch_test_macros.hpp>

#include <whirlwind/math/math.hpp>

namespace {

namespace ww = whirlwind;

CATCH_TEST_CASE("is_even", "[math]")
{
    CATCH_STATIC_REQUIRE(ww::is_even(0));
    CATCH_STATIC_REQUIRE(ww::is_even(-0));
    CATCH_STATIC_REQUIRE(ww::is_even(2));
    CATCH_STATIC_REQUIRE(ww::is_even(-1'000'000'000LL));
    CATCH_STATIC_REQUIRE(!ww::is_even(1));
    CATCH_STATIC_REQUIRE(!ww::is_even(-1));
    CATCH_STATIC_REQUIRE(!ww::is_even(1'000'000'001LL));
}

} // namespace
