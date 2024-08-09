#pragma once

#include <concepts>
#include <utility>

#include <whirlwind/common/namespace.hpp>

WHIRLWIND_NAMESPACE_BEGIN

template<class T, class First, class Second>
concept PairLike = requires(T t) {
    requires std::tuple_size_v<T> == 2;
    { std::get<0>(t) } -> std::common_reference_with<First>;
    { std::get<1>(t) } -> std::common_reference_with<Second>;
};

WHIRLWIND_NAMESPACE_END
