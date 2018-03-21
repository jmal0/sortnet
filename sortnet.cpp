#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>
#include <utility>

#if( __cplusplus <= 201103L )
    #define CXX14_CONSTEXPR inline
#else
    #define CXX14_CONSTEXPR constexpr
#endif

namespace detail
{

template <uint_fast32_t pos>
struct sortnet_swap
{
    template <typename FwdIt>
    explicit CXX14_CONSTEXPR sortnet_swap(FwdIt it)
    {
        if (*it > *std::next(it))
        {
            std::swap(*it, *std::next(it));
        }
        sortnet_swap<pos - 1>{std::next(it)};
    }
};

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

template <>
struct sortnet_swap<1>
{
    template <typename FwdIt>
    explicit CXX14_CONSTEXPR sortnet_swap(FwdIt)
    {
    }
};

template <>
struct sortnet_swap<0>
{
    template <typename FwdIt>
    explicit CXX14_CONSTEXPR sortnet_swap(FwdIt)
    {
    }
};

template <uint_fast32_t size>
struct bubble_sortnet
{
    template <typename FwdIt>
    explicit CXX14_CONSTEXPR bubble_sortnet(FwdIt first)
    {
        sortnet_swap<size>{first};
        bubble_sortnet<size - 1>{first};
    }
};

template <>
struct bubble_sortnet<0>
{
    template <typename RandIt>
    explicit CXX14_CONSTEXPR bubble_sortnet(RandIt)
    {
    }
};

// Recursive template function to create sorting networks for sizes up to net_size
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

} // namespace detail

template <typename FwdIt, uint8_t size>
CXX14_CONSTEXPR void sortnet_sort(FwdIt first)
{
    detail::bubble_sortnet<size>(first, size);
}

// 10x slower and takes 5ever to compile
template <typename FwdIt, uint8_t max_size = 255>
void sortnet_sort(FwdIt first, size_t size)
{
    if (size <= max_size)
    {
        detail::sortnet_switch<max_size>(first, size);
    }
    else
    {
        throw std::invalid_argument("size > max_size");
    }
}

template <typename T>
int test_sortnet(uint8_t max_size = 255)
{
    size_t n_tests = 100;
    size_t n_fails = 0;

    std::mt19937_64 gen(std::random_device{}());
    std::uniform_int_distribution<uint8_t> dist(0, 255);

    for (size_t i = 0; i < n_tests; ++i)
    {
        for (size_t sz = max_size; sz > 0; --sz)
        {
            std::unique_ptr<T[]> nums(new T[sz]);
            std::generate(nums.get(), nums.get() + sz, [&]() { return dist(gen); });

            sortnet_sort(nums.get(), sz);
            if (!std::is_sorted(nums.get(), nums.get() + sz))
            {
                ++n_fails;
            }
        }
    }

    std::cout << n_fails << "\n";

    return n_fails;
}

template <typename T>
void benchmark_sortnet_switch()
{
    size_t n_tests = 10000;

    std::mt19937_64 gen(std::random_device{}());
    std::uniform_int_distribution<uint8_t> dist(0, 255);

    long t_sortnet = 0;
    long t_sort = 0;

    for (size_t i = 0; i < n_tests; ++i)
    {
        const uint8_t sz = std::max<uint8_t>(2, dist(gen));
        std::unique_ptr<T[]> nums(new T[sz]);

        std::generate(nums.get(), nums.get() + sz, [&]() { return dist(gen); });
        const auto t_0_0 = std::chrono::steady_clock::now();
        sortnet_sort(nums.get(), sz);
        const auto t_1_0 = std::chrono::steady_clock::now();
        t_sortnet += std::chrono::duration_cast<std::chrono::nanoseconds>(t_1_0 - t_0_0).count();

        std::generate(nums.get(), nums.get() + sz, [&]() { return dist(gen); });
        const auto t_0_1 = std::chrono::steady_clock::now();
        std::sort(nums.get(), nums.get() + sz);
        const auto t_1_1 = std::chrono::steady_clock::now();
        t_sort += std::chrono::duration_cast<std::chrono::nanoseconds>(t_1_1 - t_0_1).count();
    }

    std::cout << "Average time (ns)\n";
    std::cout << "Sortnet   " << (t_sortnet / n_tests) << "\n";
    std::cout << "std::sort " << (t_sort / n_tests) << "\n";
}

template <typename T>
void benchmark_sortnet()
{
    size_t n_tests = 10000;

    std::mt19937_64 gen(std::random_device{}());
    std::uniform_int_distribution<uint8_t> dist(0, 255);

    long t_sortnet = 0;
    long t_sort = 0;

    constexpr uint8_t sz = 25;

    for (size_t i = 0; i < n_tests; ++i)
    {
        std::unique_ptr<T[]> nums(new T[sz]);

        std::generate(nums.get(), nums.get() + sz, [&]() { return dist(gen); });
        const auto t_0_0 = std::chrono::steady_clock::now();
        detail::bubble_sortnet<sz>(nums.get());
        const auto t_1_0 = std::chrono::steady_clock::now();
        t_sortnet += std::chrono::duration_cast<std::chrono::nanoseconds>(t_1_0 - t_0_0).count();

        std::generate(nums.get(), nums.get() + sz, [&]() { return dist(gen); });
        const auto t_0_1 = std::chrono::steady_clock::now();
        std::sort(nums.get(), nums.get() + sz);
        const auto t_1_1 = std::chrono::steady_clock::now();
        t_sort += std::chrono::duration_cast<std::chrono::nanoseconds>(t_1_1 - t_0_1).count();
    }

    std::cout << "Average time (ns)\n";
    std::cout << "Sortnet   " << (t_sortnet / n_tests) << "\n";
    std::cout << "std::sort " << (t_sort / n_tests) << "\n";
}

int main()
{
    size_t n_fails = 0;
    n_fails += test_sortnet<int>();
    n_fails += test_sortnet<unsigned>();
    n_fails += test_sortnet<float>();
    n_fails += test_sortnet<double>();

    benchmark_sortnet_switch<int>();
    benchmark_sortnet_switch<float>();

    benchmark_sortnet<int>();
    benchmark_sortnet<float>();

    return n_fails;
}
