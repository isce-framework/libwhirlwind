#pragma once

#include <deque>
#include <queue>

#include <whirlwind/common/namespace.hpp>

WHIRLWIND_NAMESPACE_BEGIN

template<class T>
using Deque = std::deque<T>;

template<class T, template<class> class Container = Deque>
class Queue : public std::queue<T, Container<T>> {
private:
    using super_type = std::queue<T, Container<T>>;

protected:
    using super_type::c;

public:
    constexpr void
    clear() noexcept
    {
        c.clear();
    }
};

WHIRLWIND_NAMESPACE_END
