#ifndef UTIL_H
#define UTIL_H

#include <stdexcept>
#include <tuple>

// assumption - [begin, end) is a non-empty range
template<typename Iterator, typename Func>
auto max_and_argmax(Iterator begin, Iterator end, Func func) -> std::tuple<decltype(func(*begin)), decltype(*begin)>
{
    auto argmax = *begin;
    auto max = func(argmax);
    ++begin;

    for(; begin != end; ++begin)
    {
        auto arg = *begin;
        auto val = func(arg);
        if (val > max)
        {
            max = val;
            argmax = arg;
        }
    }

    return std::make_tuple(max, argmax);
}

// assumption - [begin, end) is a non-empty range
template<typename Iterator, typename Func>
auto max_value(Iterator begin, Iterator end, Func func)
{
    decltype(func(*begin)) max;
    decltype(*begin) argmax;

    std::tie(max, argmax) = max_and_argmax(begin, end, func);
    return max;
}

// assumption - rng is a non-empty range
template<typename Range, typename Func>
auto max_and_argmax(const Range& rng, Func func)
{
    return max_and_argmax(rng.begin(), rng.end(), func);
}

// assumption - rng is a non-empty range
template<typename Range, typename Func>
auto max_value(const Range& rng, Func func)
{
    return max_value(rng.begin(), rng.end(), func);
}

#endif // UTIL_H

