#pragma once

#include <queue>
#include <utility>

#include <whirlwind/common/namespace.hpp>

#include "vector.hpp"

WHIRLWIND_NAMESPACE_BEGIN

namespace detail {

template<class T, class Key, template<class> class Container>
struct BinaryHeapTraits {
    using value_type = std::pair<T, Key>;

    template<class U>
    using container_type = Container<U>;

    struct value_compare {
        [[nodiscard]] constexpr auto
        operator()(const value_type& lhs, const value_type& rhs) const noexcept -> bool
        {
            using std::get;
            return get<1>(rhs) < get<1>(lhs);
        }
    };

    using queue_type =
            std::priority_queue<value_type, container_type<value_type>, value_compare>;
};

} // namespace detail

template<class T, class Key, template<class> class Container = Vector>
class BinaryHeap : public detail::BinaryHeapTraits<T, Key, Container>::queue_type {
private:
    using super_type = typename detail::BinaryHeapTraits<T, Key, Container>::queue_type;

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
