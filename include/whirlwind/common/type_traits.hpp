#pragma once

#include <iterator>
#include <type_traits>
#include <utility>

#include "namespace.hpp"

WHIRLWIND_NAMESPACE_BEGIN

template<class T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

WHIRLWIND_NAMESPACE_END
