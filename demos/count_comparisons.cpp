
#define QUICKSORT_COUNT_COMPARISONS
#include "include/algorithms.hpp"

#include <vector>
#include <numeric>
#include <algorithm>
#include <string_view>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>

using datatype = unsigned int;

long unsigned int solve(std::string filename, my::internal::AlgoConfig::QuicksortPivotChoice pivot)
{
    my::n_comparisons = 0;

    std::vector<datatype> data;
    data.reserve(100'000);

    auto file = std::ifstream(filename);

    if (file.bad()) {
        std::cerr << "Failed to open file" << std::endl;
        throw std::invalid_argument{filename};
    }

    while (!file.eof()) {
        datatype num;
        file >> num;
        data.push_back(num);
    }

    {
        using namespace my::internal;
        AlgoConfig::QUICKSORT_PIVOT_CHOICE = pivot;
        AlgoConfig::QUICKSORT_MIN_SIZE = 2;
    }

    my::quick_sort(data.begin(), data.end());
    return my::n_comparisons;
}

std::array<long unsigned, 3> read_solution(std::string const& filename)
{
    auto file = std::ifstream(filename);

    if (file.bad()) {
        std::cerr << "Failed to open file" << std::endl;
        throw std::invalid_argument{filename};
    }

    long unsigned a, b, c;
    file >> a >> b >> c;
    return {a, b, c};
}

bool test()
{
    namespace fs = std::filesystem;

    auto data = fs::directory_iterator("data/comparisons/data");
    auto solutions = fs::directory_iterator("data/comparisons/solutions");

    std::vector<std::string> data_files;
    std::vector<std::string> solu_files;
    for (auto datafile : data) {
        data_files.push_back(datafile.path().string());
    }
    for (auto solufile : solutions) {
        solu_files.push_back(solufile.path().string());
    }

    std::sort(data_files.begin(), data_files.end());
    std::sort(solu_files.begin(), solu_files.end());

    auto complaints =
      std::inner_product(data_files.cbegin(), data_files.cend(), solu_files.cbegin(), std::string{},
                         std::plus<std::string>{}, [](auto dataf, auto solutionf) -> std::string {
                             std::array<long long int, 3> counts;
                             counts[0] = solve(dataf, my::internal::AlgoConfig::QuicksortPivotChoice::FIRST);
                             counts[1] = solve(dataf, my::internal::AlgoConfig::QuicksortPivotChoice::LAST);
                             counts[2] = solve(dataf, my::internal::AlgoConfig::QuicksortPivotChoice::SAMPLE_3);

                             auto solutions = read_solution(solutionf);

                             const auto it = std::mismatch(counts.cbegin(), counts.cend(), solutions.cbegin());

                             if (it.first != counts.cend()) {
                                 std::stringstream ss;
                                 ss << "Mismatch in file " << solutionf << " (case "
                                    << 1 + std::distance(counts.cbegin(), it.first) << ")";
                                 ss << " [" << counts[0] << ", " << counts[1] << ", " << counts[2] << "] != ";
                                 ss << " [" << solutions[0] << ", " << solutions[1] << ", " << solutions[2] << "]\n";
                                 return ss.str();
                             }
                             return "";
                         });
    if (complaints.empty()) {
        return true;
    } else {
        std::cout << complaints;
        return false;
    }
}

int main()
{
    if (!test()) {
        return 1;
    }

    std::array<long long int, 3> counts;
    counts[0] = solve("data/comparisons-data.txt", my::internal::AlgoConfig::QuicksortPivotChoice::FIRST);
    counts[1] = solve("data/comparisons-data.txt", my::internal::AlgoConfig::QuicksortPivotChoice::LAST);
    counts[2] = solve("data/comparisons-data.txt", my::internal::AlgoConfig::QuicksortPivotChoice::SAMPLE_3);

    std::cout << "FIRST:    " << counts[0] << '\n';
    std::cout << "LAST:     " << counts[1] << '\n';
    std::cout << "SAMPLE_3: " << counts[2] << '\n';
    std::cout << std::endl;
}