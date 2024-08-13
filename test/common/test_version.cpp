#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <whirlwind/common/version.hpp>

namespace {

namespace CM = Catch::Matchers;

CATCH_TEST_CASE("WHIRLWIND_VERSION_EPOCH", "[version]")
{
    using T = decltype(WHIRLWIND_VERSION_EPOCH);
    CATCH_STATIC_REQUIRE((std::is_same_v<T, unsigned long>));
    CATCH_STATIC_REQUIRE(WHIRLWIND_VERSION_EPOCH >= 20240101UL);
    CATCH_STATIC_REQUIRE(WHIRLWIND_VERSION_EPOCH <= 99999999UL);
}

CATCH_TEST_CASE("WHIRLWIND_VERSION_PATCH", "[version]")
{
    CATCH_STATIC_REQUIRE((std::is_same_v<decltype(WHIRLWIND_VERSION_PATCH), unsigned>));
    CATCH_STATIC_REQUIRE(WHIRLWIND_VERSION_PATCH >= 0U);
}

CATCH_TEST_CASE("WHIRLWIND_VERSION_STRING", "[version]")
{
    CATCH_CHECK_THAT(WHIRLWIND_VERSION_STRING, CM::Matches(R"(^\d{8}\.\d+$)"));
}

} // namespace
