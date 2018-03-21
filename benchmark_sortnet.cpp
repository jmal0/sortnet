#include <algorithm>
#include <cstdint>
#include <chrono>
#include <iostream>
#include <memory>
#include <random>

#include <sortnet.h>

template <typename T>
void benchmark_sortnet_switch(size_t n_reps = 1000)
{
    std::mt19937_64 gen(std::random_device{}());
    std::uniform_int_distribution<uint8_t> dist(0, 255);

    for (uint8_t sz = 255; sz > 1; sz -= 5)
    {
        long t_sortnet = 0;
        long t_sort = 0;
        for (size_t i = 0; i < n_reps; ++i)
        {
            std::unique_ptr<T[]> nums(new T[sz]);

            std::generate(nums.get(), nums.get() + sz, [&]() { return dist(gen); });
            const auto t_0_0 = std::chrono::steady_clock::now();
            JMAL::sortnet_sort(nums.get(), sz);
            const auto t_1_0 = std::chrono::steady_clock::now();
            t_sortnet += std::chrono::duration_cast<std::chrono::nanoseconds>(t_1_0 - t_0_0).count();

            std::generate(nums.get(), nums.get() + sz, [&]() { return dist(gen); });
            const auto t_0_1 = std::chrono::steady_clock::now();
            std::sort(nums.get(), nums.get() + sz);
            const auto t_1_1 = std::chrono::steady_clock::now();
            t_sort += std::chrono::duration_cast<std::chrono::nanoseconds>(t_1_1 - t_0_1).count();
        }
        std::cout << "Average time (ns) to sort " << static_cast<int>(sz) << " elements\n";
        std::cout << "Sortnet   " << (t_sortnet / n_reps) << "\n";
        std::cout << "std::sort " << (t_sort / n_reps) << "\n";
    }
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
        JMAL::bubble_sortnet<sz>(nums.get());
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
    std::cout << "Benchmarking with int\n";
    benchmark_sortnet_switch<int>();
    std::cout << "Benchmarking with float\n";
    benchmark_sortnet_switch<float>();

    std::cout << "Benchmarking with int and array length of 25\n";
    benchmark_sortnet<int>();
    std::cout << "Benchmarking with float and array length of 25\n";
    benchmark_sortnet<float>();

    return 0;
}
