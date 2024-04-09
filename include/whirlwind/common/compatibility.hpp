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

#if defined(__has_cpp_attribute) && __has_cpp_attribute(likely) >= 201803L
#define WHIRLWIND_LIKELY [[likely]]
#else
#define WHIRLWIND_LIKELY
#endif

#if defined(__has_cpp_attribute) && __has_cpp_attribute(unlikely) >= 201803L
#define WHIRLWIND_UNLIKELY [[unlikely]]
#else
#define WHIRLWIND_UNLIKELY
#endif

/**
 * @def WHIRLWIND_STRINGIFY(s)
 *
 * Converts the argument into a string literal. If the argument is a macro, it is not
 * expanded.
 */
#define WHIRLWIND_STRINGIFY(s) #s

/**
 * @def WHIRLWIND_UNROLL(count)
 *
 * Hints to the compiler that the following loop should be unrolled `count` times (if
 * supported by the compiler).
 *
 * It must be placed immediately before a `for`, `while`, or `do` loop.
 */
#if defined(__GNUC__)
#define WHIRLWIND_UNROLL(count) _Pragma(WHIRLWIND_STRINGIFY(unroll GCC count))
#elif defined(__clang__)
#define WHIRLWIND_UNROLL(count) _Pragma(WHIRLWIND_STRINGIFY(unroll count))
#else
#define WHIRLWIND_UNROLL(count)
#endif
