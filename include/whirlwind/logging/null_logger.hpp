#pragma once

#include <whirlwind/common/namespace.hpp>

WHIRLWIND_NAMESPACE_BEGIN

struct NullLogger {
    template<class String>
    explicit constexpr NullLogger(const String&)
    {}

    template<class FormatString, class... Args>
    constexpr void
    trace(FormatString&&, Args&&...)
    {}

    template<class FormatString, class... Args>
    constexpr void
    debug(FormatString&&, Args&&...)
    {}

    template<class FormatString, class... Args>
    constexpr void
    info(FormatString&&, Args&&...)
    {}

    template<class FormatString, class... Args>
    constexpr void
    warn(FormatString&&, Args&&...)
    {}

    template<class FormatString, class... Args>
    constexpr void
    error(FormatString&&, Args&&...)
    {}

    template<class FormatString, class... Args>
    constexpr void
    critical(FormatString&&, Args&&...)
    {}
};

WHIRLWIND_NAMESPACE_END
