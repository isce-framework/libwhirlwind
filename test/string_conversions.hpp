#pragma once

#include <ostream>
#include <utility>

namespace std {

template<class T1, class T2>
ostream&
operator<<(ostream& os, const pair<T1, T2>& value)
{
    os << "(" << value.first << ", " << value.second << ")";
    return os;
}

} // namespace std
