#include "algorithms.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

std::vector<int> random_vector(const std::size_t size, const int max_value = std::numeric_limits<int>::max())
{
    std::vector<int> v(size);
    std::generate(v.begin(), v.end(), [=]()
                  { return rand() % max_value; });
    return v;
}

template <typename InputIterator>
void verify_sorted(const InputIterator begin, const InputIterator end)
{
    auto result = std::adjacent_find(begin, end, [](auto x, auto y)
                                     { return x > y; });

    if (result != end)
    {
        std::cerr << "Vector not properly sorted!\nMismatch in position " << std::distance(begin, result) << std::endl;
    }
    else
    {
        std::cout << "Success!" << std::endl;
    }
}

int main(int argc, char **argv)
{
    const std::size_t sample_size = [&]()
    {
        if (argc == 2)
        {
            return std::atoi(argv[1]);
        }
        std::cout << "Usage: mergesort <samplesize>\n"
                  << "\n    samplesize: Size of the list to sort [Default=1000]." << std::endl;
        return 1000;
    }();

    auto v = random_vector(sample_size, 10 * sample_size);
    my::merge_sort(v.begin(), v.end());
    verify_sorted(v.begin(), v.end());
}
/*
time ./mergesort 1000    ->    0,004s
time ./mergesort 10000   ->    0,012s
time ./mergesort 100000  ->    0,727s
time ./mergesort 1000000 ->   81,088s
*/