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

/// Component of insertion sort. Takes a sorted range [first, last) rotates moves the 'last' element to the right place.
template <typename BiderectionalIterator, typename Comparator, typename SwapCounter>
BiderectionalIterator sort_last_forward(BiderectionalIterator first,
                                        BiderectionalIterator last,
                                        Comparator comp,
                                        SwapCounter* swaps)
{
    constexpr bool count_swaps = !std::is_same_v<SwapCounter, void>;

    if (first == last) {
        return last;
    }

    auto rend = std::reverse_iterator(first);
    auto rbegin = std::reverse_iterator(last);

    return std::adjacent_find(rbegin,
                              rend,
                              [&](auto& right, auto& left) {
                                  if (!comp(left, right)) {
                                      std::swap(left, right);
                                      if constexpr (count_swaps) {
                                          ++(*swaps);
                                      }
                                      return false;
                                  }
                                  return true;
                              })
      .base();
}

// In-situ insertion-sort
template <typename BiderectionalIterator, typename Comparator, typename SwapCounter>
void insertion_sort_impl(BiderectionalIterator begin, BiderectionalIterator end, Comparator comp, SwapCounter* swaps)
{
    auto begin_unsorted = begin;
    while (begin_unsorted != end) {
        std::advance(begin_unsorted, 1);
        sort_last_forward<BiderectionalIterator, Comparator, SwapCounter>(begin, begin_unsorted, comp, swaps);
    }
}

/// In-situ selection-sort
template <typename InputIterator, typename Comparator>
void selection_sort_impl(InputIterator begin, InputIterator end, Comparator compare)
{
    for (; begin != end; std::advance(begin, 1)) {
        auto min_elem = std::min_element(begin, end, compare);
        std::swap(*begin, *min_elem);
    }
}

// Non-recursive search algorithm metaprogramming
template <typename InputIterator, typename Comparator, typename SwapCounter>
void non_recursive_sort(InputIterator begin, InputIterator end, Comparator compare, SwapCounter* swaps)
{
    if constexpr (std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category,
                                      std::bidirectional_iterator_tag>::value) {
        insertion_sort_impl<InputIterator, Comparator, SwapCounter>(begin, end, compare, swaps);
    } else {
        static_assert(std::is_same_v<SwapCounter, void>,
                      "Swap counter is not supported for non-bidirectional iterators");
        selection_sort_impl<InputIterator, Comparator>(begin, end, compare);
    }
}

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

namespace internal
{

/// In-situ merge-sort
template <typename InputIterator, typename Comparator, typename SwapCounter>
void merge_sort_impl(InputIterator begin, InputIterator end, Comparator compare, SwapCounter* swaps)
{
    constexpr bool count_swaps = !std::is_same_v<SwapCounter, void>;
    const auto size = std::distance(begin, end);

    if (size < internal::MERGESORT_MIN_SIZE) {
        non_recursive_sort<InputIterator, Comparator, SwapCounter>(begin, end, compare, swaps);
        return;
    }

    auto middle = std::next(begin, size / 2);
    merge_sort_impl<InputIterator, Comparator, SwapCounter>(begin, middle, compare, swaps);
    merge_sort_impl<InputIterator, Comparator, SwapCounter>(middle, end, compare, swaps);

    // Merging
    while (begin != middle && middle != end) {
        if (!compare(*begin, *middle)) {
            auto new_middle = sorted_partition_point(middle, end, [&](auto x) { return compare(x, *begin); });

            if constexpr (count_swaps) {
                *swaps += std::distance(middle, new_middle) * std::distance(begin, middle);
            }

            begin = std::rotate(begin, middle, new_middle);
            middle = new_middle;
        }
        std::advance(begin, 1);
    }
}

}

template <typename InputIterator,
          typename Comparator = std::less<typename std::iterator_traits<InputIterator>::value_type>>
void insertion_sort(InputIterator begin, InputIterator end, Comparator compare = Comparator{})
{
    internal::insertion_sort_impl<InputIterator, Comparator, void>(begin, end, compare, nullptr);
}

template <typename InputIterator,
          typename Comparator = std::less<typename std::iterator_traits<InputIterator>::value_type>>
void selection_sort(InputIterator begin, InputIterator end, Comparator compare = Comparator{})
{
    internal::selection_sort_impl<InputIterator, Comparator>(begin, end, compare);
}

/// In-situ merge-sort
template <typename InputIterator,
          typename Comparator = std::less<typename std::iterator_traits<InputIterator>::value_type>>
void merge_sort(InputIterator begin, InputIterator end, Comparator compare = Comparator{})
{
    internal::merge_sort_impl<InputIterator, Comparator, void>(begin, end, compare, nullptr);
}

/// In-situ merge-sort + inversion count
template <typename InputIterator,
          typename Comparator = std::less<typename std::iterator_traits<InputIterator>::value_type>>
std::size_t sort_and_count_inversions(InputIterator begin, InputIterator end, Comparator compare = Comparator{})
{
    static_assert(std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category,
                                      std::bidirectional_iterator_tag>::value,
                  "InputIterator must be bidirectional");

    std::size_t swaps = 0;
    internal::merge_sort_impl<InputIterator, Comparator, std::size_t>(begin, end, compare, &swaps);
    return swaps;
}

}