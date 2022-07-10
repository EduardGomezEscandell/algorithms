#pragma once

#include <functional>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <numeric>

namespace my
{

constexpr std::ptrdiff_t MERGESORT_MIN_SIZE = 100;
constexpr std::ptrdiff_t BINSEARCH_MIN_SIZE = 100;

#ifndef NDEBUG
constexpr bool DEBUG = true;
#else
constexpr bool DEBUG = false;
#endif

template <typename Iterator>
using iterator_value_t = typename std::iterator_traits<Iterator>::value_type;

template <typename InputIterator, typename UnaryPredicate>
InputIterator sorted_partition_point(InputIterator begin, InputIterator end, UnaryPredicate pred)
{
    const auto size = std::distance(begin, end);
    if (size < BINSEARCH_MIN_SIZE) {
        return std::partition_point(begin, end, pred);
    }

    const auto midpoint = std::next(begin, size / 2);
    if (pred(*midpoint)) {
        return sorted_partition_point(begin, midpoint, pred);
    }
    return sorted_partition_point(midpoint, end, pred);
}

template <typename InputIterator>
void display(InputIterator begin, const InputIterator end, std::string_view endstr = "\n")
{
    std::stringstream ss;
    for (; begin != end; std::advance(begin, 1)) {
        ss << *begin << ' ';
    }
    std::string s = ss.str();
    std::string_view v{s.c_str(), s.size() > 0 ? s.size() - 1 : 0};
    std::cout << v << endstr;
}

template <typename InputIterator, typename Comparator = std::less<iterator_value_t<InputIterator>>>
void insertion_sort(InputIterator begin, InputIterator end, Comparator compare = Comparator{})
{
    for (; begin != end; std::advance(begin, 1)) {
        std::swap(*begin, *std::min_element(begin, end, compare));
    }
}

/// In-situ merge-sort
template <typename InputIterator, typename Comparator = std::less<iterator_value_t<InputIterator>>>
void merge_sort(InputIterator begin, InputIterator end, Comparator compare = Comparator{})
{
    const auto size = std::distance(begin, end);

    if (size < MERGESORT_MIN_SIZE) {
        insertion_sort(begin, end, compare);
        return;
    }

    auto middle = std::next(begin, size / 2);
    merge_sort(begin, middle);
    merge_sort(middle, end);

    [[maybe_unused]] const auto old_begin = begin;

    // Merging
    while (begin != middle && middle != end) {
        if (!compare(*begin, *middle)) {
            auto new_middle = sorted_partition_point(middle, end, [&](auto x) { return x == *begin; });
            begin = std::rotate(begin, middle, new_middle);
            middle = new_middle;
        }
        std::advance(begin, 1);

        if constexpr (DEBUG) {
            display(old_begin, begin, "[");
            display(begin, middle, "]");
            display(middle, end, "\n");
        }
    }
}
}