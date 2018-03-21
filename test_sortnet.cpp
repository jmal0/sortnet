#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <random>

#include <sortnet.h>

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

            JMAL::sortnet_sort(nums.get(), sz);
            if (!std::is_sorted(nums.get(), nums.get() + sz))
            {
                ++n_fails;
            }
        }
    }

    std::cout << n_fails << "\n";

    return n_fails;
}

int main()
{
    size_t n_fails = 0;
    n_fails += test_sortnet<int>();
    n_fails += test_sortnet<unsigned>();
    n_fails += test_sortnet<float>();
    n_fails += test_sortnet<double>();

    return n_fails;
}
