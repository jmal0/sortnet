#ifndef _JMAL_SORTNET_H
#define _JMAL_SORTNET_H

#include <algorithm> // std::swap
#include <cstdint>   // uint8_t
#include <iterator>  // std::next
#include <stdexcept> // std::invalid_argument

#if( __cplusplus <= 201103L )
    #define CXX14_CONSTEXPR inline
#else
    #define CXX14_CONSTEXPR constexpr
#endif

// @todo add comparator

namespace JMAL
{

template <uint8_t size>
struct sortnet_swap
{
    // Move max value in range [it, it + size) to last position
    template <typename FwdIt>
    explicit CXX14_CONSTEXPR sortnet_swap(FwdIt it)
    {
        if (*it > *std::next(it))
        {
            std::swap(*it, *std::next(it));
        }
        sortnet_swap<size - 1>{std::next(it)};
    }
};

// terminate recursion
template <>
struct sortnet_swap<2>
{
    template <typename FwdIt>
    explicit CXX14_CONSTEXPR sortnet_swap(FwdIt it)
    {
        if (*it > *std::next(it))
        {
            std::swap(*it, *(std::next(it)));
        }
    }
};

// specialization if given size == 1 (no sorting to do)
template <>
struct sortnet_swap<1>
{
    template <typename FwdIt>
    explicit CXX14_CONSTEXPR sortnet_swap(FwdIt)
    {
    }
};

// specialization if given size == 0 (no sorting to do)
template <>
struct sortnet_swap<0>
{
    template <typename FwdIt>
    explicit CXX14_CONSTEXPR sortnet_swap(FwdIt)
    {
    }
};

// Recursive template to generate sorting network for input container of given size
template <uint8_t size>
struct bubble_sortnet
{
    // Perform bubble sort on the range [first, first + size) using sortnet
    template <typename FwdIt>
    explicit CXX14_CONSTEXPR bubble_sortnet(FwdIt first)
    {
        sortnet_swap<size>{first};
        bubble_sortnet<size - 1>{first};
    }
};

// terminate recursion (no more sorting to do)
template <>
struct bubble_sortnet<1>
{
    template <typename RandIt>
    explicit CXX14_CONSTEXPR bubble_sortnet(RandIt)
    {
    }
};

// specialization if given size == 0 (no sorting to do)
template <>
struct bubble_sortnet<0>
{
    template <typename RandIt>
    explicit CXX14_CONSTEXPR bubble_sortnet(RandIt)
    {
    }
};

// Recursive template function to create sort any input with size up to net_size
template <uint8_t net_size, typename FwdIt>
CXX14_CONSTEXPR void sortnet_switch(const FwdIt first, const uint8_t input_size)
{
    if (net_size == input_size)
    {
        bubble_sortnet<net_size>{first};
    }
    else if (net_size > 1)
    {
        sortnet_switch<net_size - 1>(first, input_size);
    }
}

// Wrap struct into a template function for convenience
template <typename FwdIt, uint8_t size>
CXX14_CONSTEXPR void sortnet_sort(FwdIt first)
{
    bubble_sortnet<size>{first, size};
}

// 10x slower and takes 5ever to compile
template <typename FwdIt, uint8_t max_size = 255>
void sortnet_sort(FwdIt first, uint8_t size)
{
    if (size <= max_size)
    {
        sortnet_switch<max_size>(first, size);
    }
    else
    {
        throw std::invalid_argument("size > max_size");
    }
}

} // namespace JMAL

#endif // _JMAL_SORTNET_H
