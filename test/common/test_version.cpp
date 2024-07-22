#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <whirlwind/common/version.hpp>

namespace {

TEST_CASE("WHIRLWIND_VERSION_EPOCH", "[version]")
{
    STATIC_REQUIRE((std::is_same_v<decltype(WHIRLWIND_VERSION_EPOCH), unsigned long>));
    STATIC_REQUIRE(WHIRLWIND_VERSION_EPOCH >= 20240101UL);
    STATIC_REQUIRE(WHIRLWIND_VERSION_EPOCH <= 99999999UL);
}

TEST_CASE("WHIRLWIND_VERSION_PATCH", "[version]")
{
    STATIC_REQUIRE((std::is_same_v<decltype(WHIRLWIND_VERSION_PATCH), unsigned>));
    STATIC_REQUIRE(WHIRLWIND_VERSION_PATCH >= 0U);
}

TEST_CASE("WHIRLWIND_VERSION_STRING", "[version]")
{
    using Catch::Matchers::Matches;
    CHECK_THAT(WHIRLWIND_VERSION_STRING, Matches(R"(^\d{8}\.\d+$)"));
}

} // namespace
