#include "include/algorithms.hpp"

#include <algorithm>
#include <string_view>
#include <vector>
#include <iostream>
#include <fstream>

int main()
{
    std::vector<unsigned int> data;
    data.reserve(100'000);

    auto file = std::ifstream("data/numbers-1-to-100k-unsorted.txt");

    if (file.bad()) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    while (!file.eof()) {
        unsigned int num;
        file >> num;
        data.push_back(num);
    }
    std::cout << "Read " << data.size() << " values" << std::endl;
    auto inversions = my::sort_and_count_inversions(data.begin(), data.end());
    std::cout << "Found " << inversions << " inversions" << std::endl;
}