#pragma once

#include <functional>
#include <algorithm>
#include <numeric>

namespace my
{

namespace internal
{
constexpr std::ptrdiff_t MERGESORT_MIN_SIZE = 100;
constexpr std::ptrdiff_t BINSEARCH_MIN_SIZE = 100;
}

template <typename InputIterator, typename UnaryPredicate>
InputIterator sorted_partition_point(InputIterator begin, InputIterator end, UnaryPredicate pred)
{
    const auto size = std::distance(begin, end);
    if (size < internal::BINSEARCH_MIN_SIZE) {
        return std::partition_point(begin, end, pred);
    }

    const auto midpoint = std::next(begin, size / 2);
    if (pred(*midpoint)) {
        return sorted_partition_point(midpoint, end, pred);
    }
    return sorted_partition_point(begin, midpoint, pred);
}

/// In-situ insertion-sort
template <typename InputIterator,
          typename Comparator = std::less<typename std::iterator_traits<InputIterator>::value_type>>
void insertion_sort(InputIterator begin, InputIterator end, Comparator compare = Comparator{})
{
    for (; begin != end; std::advance(begin, 1)) {
        std::swap(*begin, *std::min_element(begin, end, compare));
    }
}

/// In-situ merge-sort
template <typename InputIterator,
          typename Comparator = std::less<typename std::iterator_traits<InputIterator>::value_type>>
void merge_sort(InputIterator begin, InputIterator end, Comparator compare = Comparator{})
{
    const auto size = std::distance(begin, end);

    if (size < internal::MERGESORT_MIN_SIZE) {
        insertion_sort(begin, end, compare);
        return;
    }

    auto middle = std::next(begin, size / 2);
    merge_sort(begin, middle, compare);
    merge_sort(middle, end, compare);

    // Merging
    while (begin != middle && middle != end) {
        if (!compare(*begin, *middle)) {
            auto new_middle = sorted_partition_point(middle, end, [&](auto x) { return compare(x, *begin); });
            begin = std::rotate(begin, middle, new_middle);
            middle = new_middle;
        }
        std::advance(begin, 1);
    }
}
}