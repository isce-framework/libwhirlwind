#pragma once

#if defined(__cpp_consteval) && __cpp_consteval >= 201811L
#define WHIRLWIND_CONSTEVAL consteval
#else
#define WHIRLWIND_CONSTEVAL constexpr
#endif

#if defined(__has_cpp_attribute) && __has_cpp_attribute(no_unique_address) >= 201803L
#define WHIRLWIND_NO_UNIQUE_ADDRESS [[no_unique_address]]
#else
#define WHIRLWIND_NO_UNIQUE_ADDRESS
#endif

#if defined(__has_cpp_attribute) && __has_cpp_attribute(unlikely) >= 201803L
#define WHIRLWIND_UNLIKELY [[unlikely]]
#else
#define WHIRLWIND_UNLIKELY
#endif
