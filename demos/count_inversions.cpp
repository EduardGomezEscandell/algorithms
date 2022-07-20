#include "include/algorithms.hpp"

#include <algorithm>
#include <string_view>
#include <vector>
#include <iostream>
#include <fstream>

int main()
{
    using datatype = unsigned int;

    std::vector<datatype> data;
    data.reserve(100'000);

    auto file = std::ifstream("data/inversions-data.txt");

    if (file.bad()) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    while (!file.eof()) {
        datatype num;
        file >> num;
        data.push_back(num);
    }
    std::cout << "Read " << data.size() << " values" << std::endl;
    auto ouput = std::vector<datatype>(data.size(), 0);

    auto inversions = my::sort_and_count_inversions(data.cbegin(), data.cend(), ouput.begin());
    std::cout << "Found " << inversions << " inversions" << std::endl;
}