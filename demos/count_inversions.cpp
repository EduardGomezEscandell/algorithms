#include "include/algorithms.hpp"

#include <string_view>
#include <vector>
#include <iostream>

int main(int argc, char** argv)
{
    std::vector<std::string_view> args(argv + 1, argv + argc);

    const auto inversion_count = my::sort_and_count_inversions(args.begin(), args.end());
    std::cout << "Found " << inversion_count << " inversions." << std::endl;
    std::cout << "Sorted: ";
    std::for_each(args.cbegin(), args.cend(), [](auto& s) { std::cout << s << " "; });
    std::cout << std::endl;
}