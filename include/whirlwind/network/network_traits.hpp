#pragma once

#include <utility>

#include <whirlwind/common/namespace.hpp>
#include <whirlwind/common/type_traits.hpp>

WHIRLWIND_NAMESPACE_BEGIN

template<class Network>
struct NetworkTraits {
    using network_type = Network;
    using node_type = typename network_type::node_type;
    using arc_type = typename network_type::arc_type;
    using size_type = typename network_type::size_type;
    using nodes_type = remove_cvref_t<decltype(std::declval<network_type>().nodes())>;
    using arcs_type = remove_cvref_t<decltype(std::declval<network_type>().arcs())>;
    using outgoing_arcs_type =
            remove_cvref_t<decltype(std::declval<network_type>().outgoing_arcs(
                    std::declval<node_type>()))>;
};

WHIRLWIND_NAMESPACE_END
