#pragma once

#include <iterator>
#include <type_traits>
#include <utility>

#include "namespace.hpp"

WHIRLWIND_NAMESPACE_BEGIN

template<class T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

namespace ranges {

template<class Range>
using iterator_type = remove_cvref_t<decltype(std::begin(std::declval<Range>()))>;

template<class Range>
using size_type = typename Range::size_type;

template<class Range>
using value_type = remove_cvref_t<decltype(*std::begin(std::declval<Range>()))>;

} // namespace ranges

WHIRLWIND_NAMESPACE_END
