#pragma once

#include "version.hpp"

/**
 * @def WHIRLWIND_ABI_NAMESPACE
 *
 * The name of the inline ABI namespace.
 */
#define WHIRLWIND_MAKE_ABI_NAMESPACE_IMPL(epoch, patch) abi_##epoch##_##patch
#define WHIRLWIND_MAKE_ABI_NAMESPACE(epoch, patch)                                     \
    WHIRLWIND_MAKE_ABI_NAMESPACE_IMPL(epoch, patch)
#define WHIRLWIND_ABI_NAMESPACE                                                        \
    WHIRLWIND_MAKE_ABI_NAMESPACE(WHIRLWIND_VERSION_EPOCH, WHIRLWIND_VERSION_PATCH)

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
