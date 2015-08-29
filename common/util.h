#ifndef UTIL_H
#define UTIL_H

#include <stdexcept>

// assumption - [begin, end) is a non-empty range
template<typename Iterator, typename KeyFunc>
auto max_key(Iterator begin, Iterator end, KeyFunc key) -> decltype(key(*begin))
{
    auto max = key(*begin);
    ++begin;

    for(; begin != end; ++begin)
    {
        auto candidate = key(*begin);
        if (candidate > max)
            candidate = max;
    }

    return candidate;
}

template<typename Range, typename KeyFunc>
auto max_key(Range rng, KeyFunc key)
{
    return max_key(begin(rng), end(rng), key);
}

#endif // UTIL_H

