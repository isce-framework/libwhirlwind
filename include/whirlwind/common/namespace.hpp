#pragma once

#include "version.hpp"

/**
 * @def WHIRLWIND_CONCATENATE(...)
 *
 * Concatenate zero or more (up to eight) tokens. Macros are expanded.
 */
#define WHIRLWIND_CONCATENATE_(a, b, c, d, e, f, g, h, ...) a##b##c##d##e##f##g##h
#define WHIRLWIND_CONCATENATE(...) WHIRLWIND_CONCATENATE_(__VA_ARGS__, , , , , , , , )

/**
 * @def WHIRLWIND_ABI_NAMESPACE
 *
 * The name of the inline ABI namespace.
 */
#define WHIRLWIND_ABI_NAMESPACE                                                        \
    WHIRLWIND_CONCATENATE(abi_, WHIRLWIND_VERSION_MAJOR, _, WHIRLWIND_VERSION_MINOR)

/**
 * @def WHIRLWIND_NAMESPACE
 *
 * The full name of the `whirlwind` namespace, including any enclosed inline namespaces.
 */
#define WHIRLWIND_NAMESPACE whirlwind::WHIRLWIND_ABI_NAMESPACE

/**
 * @def WHIRLWIND_NAMESPACE_BEGIN
 *
 * Begins a `whirlwind` namespace block, including any enclosed inline namespaces.
 */
#define WHIRLWIND_NAMESPACE_BEGIN                                                      \
    namespace whirlwind {                                                              \
    inline namespace WHIRLWIND_ABI_NAMESPACE {

/**
 * @def WHIRLWIND_NAMESPACE_END
 *
 * Ends a `whirlwind` namespace block, including any enclosed inline namespaces.
 */
#define WHIRLWIND_NAMESPACE_END                                                        \
    }                                                                                  \
    }
