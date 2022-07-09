#pragma once

#include <functional>
#include <sstream>
#include <iostream>

namespace my
{

constexpr std::ptrdiff_t MERGESORT_MIN_SIZE = 100;
constexpr std::ptrdiff_t BINSEARCH_MIN_SIZE = 100;

#ifndef NDEBUG
constexpr bool DEBUG = true;
#else
constexpr bool DEBUG = false;
#endif

template <typename Iterator> using ValueType = typename std::iterator_traits<Iterator>::value_type;

template <typename InputIterator, typename Comparator = std::less<ValueType<InputIterator>>>
InputIterator min_element(InputIterator begin, InputIterator end, Comparator compare = Comparator{})
{
    auto min_ = begin;
    for (std::advance(begin, 1); begin != end; std::advance(begin, 1)) {
        if (compare(*begin, *min_)) {
            min_ = begin;
        }
    }
    return min_;
}

template <typename InputIterator, typename Comparator = std::less<ValueType<InputIterator>>>
InputIterator partition_point(InputIterator begin,
                              InputIterator end,
                              ValueType<InputIterator> const &value,
                              Comparator compare = Comparator{})
{
    for (; begin != end; std::advance(begin, 1)) {
        if (!compare(*begin, value)) {
            return begin;
        }
    }
    return end;
}

template <typename InputIterator, typename Comparator = std::less<ValueType<InputIterator>>>
InputIterator sorted_partition_point(InputIterator begin,
                                     InputIterator end,
                                     ValueType<InputIterator> const &value,
                                     Comparator compare = Comparator{})
{
    const auto size = std::distance(begin, end);
    if (size < BINSEARCH_MIN_SIZE) {
        return partition_point(begin, end, value, compare);
    }

    const auto midpoint = std::next(begin, size / 2);
    if (compare(value, *midpoint)) {
        return sorted_partition_point(begin, midpoint, value, compare);
    }
    return sorted_partition_point(midpoint, end, value, compare);
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

template <typename InputIterator, typename Comparator = std::less<ValueType<InputIterator>>>
void insertion_sort(InputIterator begin, InputIterator end, Comparator compare = Comparator{})
{
    for (; begin != end; std::advance(begin, 1)) {
        std::swap(*begin, *my::min_element(begin, end, compare));
    }
}

/// In-situ merge-sort
template <typename InputIterator, typename Comparator = std::less<ValueType<InputIterator>>>
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
            auto new_middle = sorted_partition_point(middle, end, *begin, compare);
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