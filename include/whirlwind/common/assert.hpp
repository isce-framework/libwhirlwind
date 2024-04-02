#pragma once

#include <cassert>

/**
 * @def WHIRLWIND_ASSERT(condition)
 *
 * Conditional assertion. Outputs diagnostic information and aborts if `condition` is
 * false. May be disabled in non-debug builds.
 *
 * The default behavior may be overridden by defining this macro before including any
 * `whirlwind` header files.
 */
#ifndef WHIRLWIND_ASSERT
#define WHIRLWIND_ASSERT(condition) assert(condition)
#endif

/**
 * @def WHIRLWIND_DEBUG_ASSERT(condition)
 *
 * Conditional assertion. Outputs diagnostic information and aborts if `condition` is
 * false. Has no effect in non-debug builds.
 *
 * The default behavior may be overridden by defining this macro before including any
 * `whirlwind` header files.
 */
#ifndef WHIRLWIND_DEBUG_ASSERT
#define WHIRLWIND_DEBUG_ASSERT(condition) assert(condition)
#endif

/**
 * @def WHIRLWIND_STATIC_ASSERT(...)
 *
 * Declares a static assertion. If the assertion fails, the program is ill-formed, and a
 * diagnostic error message may be generated.
 *
 * The default behavior may be overridden by defining this macro before including any
 * `whirlwind` header files.
 */
#ifndef WHIRLWIND_STATIC_ASSERT
#define WHIRLWIND_STATIC_ASSERT(...) static_assert(__VA_ARGS__)
#endif
