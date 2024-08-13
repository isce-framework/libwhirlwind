#pragma once

#include <functional>
#include <sstream>
#include <string>
#include <utility>

#include <catch2/matchers/catch_matchers_templated.hpp>
#include <range/v3/algorithm/all_of.hpp>

#include <whirlwind/common/namespace.hpp>

WHIRLWIND_NAMESPACE_BEGIN
namespace testing {

template<class Value, class Pred = std::equal_to<>>
class AllEqualTo : public Catch::Matchers::MatcherGenericBase {
public:
    explicit constexpr AllEqualTo(Value value, Pred pred = {}) noexcept
        : value_(std::move(value)), pred_(std::move(pred))
    {}

    template<class Range>
    [[nodiscard]] constexpr auto
    match(Range&& r) const -> bool
    {
        return ranges::all_of(std::forward<Range>(r),
                              [&](const auto& item) { return pred_(item, value_); });
    }

    [[nodiscard]] auto
    describe() const -> std::string override
    {
        std::stringstream ss;
        ss << "all elements in range are equal to " << value_;
        return std::move(ss).str();
    }

private:
    Value value_;
    [[no_unique_address]] Pred pred_;
};

template<class T>
AllEqualTo(const T&) -> AllEqualTo<T>;

template<class T>
AllEqualTo(T&&) -> AllEqualTo<T>;

} // namespace testing
WHIRLWIND_NAMESPACE_END
