#pragma once

#include <functional>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <pstl/glue_execution_defs.h>

namespace my
{

namespace internal
{
constexpr std::ptrdiff_t MERGESORT_MIN_SIZE = 100;
constexpr std::ptrdiff_t BINSEARCH_MIN_SIZE = 100;
constexpr std::ptrdiff_t QUICKSORT_MIN_SIZE = 100;

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
        std::iter_swap(begin, min_elem);
    }
}

// Non-recursive search algorithm metaprogramming
template <typename Iterator, typename Comparator, typename SwapCounter>
void non_recursive_sort(Iterator begin, Iterator end, Comparator compare, SwapCounter* swaps)
{
    if constexpr (std::is_convertible<typename std::iterator_traits<Iterator>::iterator_category,
                                      std::bidirectional_iterator_tag>::value) {
        insertion_sort_impl<Iterator, Comparator, SwapCounter>(begin, end, compare, swaps);
    } else {
        static_assert(std::is_same_v<SwapCounter, void>,
                      "Swap counter is not supported for non-bidirectional iterators");
        selection_sort_impl<Iterator, Comparator>(begin, end, compare);
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

template <typename LInputIterator,
          typename RInputIterator,
          typename OutputIterator,
          typename Comparator,
          typename SwapCounter = void>
OutputIterator merge_impl(LInputIterator left_begin,
                          LInputIterator left_end,
                          RInputIterator right_begin,
                          RInputIterator right_end,
                          OutputIterator out_begin,
                          Comparator compare,
                          SwapCounter* swaps = nullptr)
{
    constexpr bool count_swaps = !std::is_same_v<SwapCounter, void>;
    [[maybe_unused]] const auto left_size = std::distance(left_begin, left_end);

    auto it = out_begin;
    auto left = left_begin;
    auto right = right_begin;

    while (left != left_end && right != right_end) {
        bool comp = compare(*left, *right);

        if constexpr (count_swaps) {
            auto from = comp ? std::distance(left_begin, left) : left_size + std::distance(right_begin, right);
            auto to = std::distance(out_begin, it);
            *swaps += from - to;
        }

        auto& ptr = comp ? left : right;
        *it = *ptr;
        std::advance(ptr, 1);
        std::advance(it, 1);
    }

    if (right == right_end) {
        return std::copy(left, left_end, it);
    }
    return std::copy(right, right_end, it);
}

/// Merge-sort
template <typename InputIterator, typename OutputIterator, typename Comparator, typename SwapCounter>
OutputIterator merge_sort_impl(InputIterator begin,
                               InputIterator end,
                               OutputIterator out_begin,
                               Comparator compare,
                               SwapCounter* swaps)
{
    const auto size = std::distance(begin, end);

    if (size < internal::MERGESORT_MIN_SIZE) {
        auto out_end = std::copy(begin, end, out_begin);
        non_recursive_sort<OutputIterator, Comparator, SwapCounter>(out_begin, out_end, compare, swaps);
        return out_end;
    }

    auto middle = std::next(begin, size / 2);

    auto intermediate_container = std::vector(begin, end);
    auto int_begin = intermediate_container.begin();
    auto int_middle = std::next(int_begin, size / 2);
    auto int_end = intermediate_container.end();

    merge_sort_impl<InputIterator, decltype(int_begin), Comparator, SwapCounter>(
      begin, middle, int_begin, compare, swaps);
    merge_sort_impl<InputIterator, decltype(int_begin), Comparator, SwapCounter>(
      middle, end, int_middle, compare, swaps);

    // Merging
    return my::internal::merge_impl(std::move_iterator(int_begin),
                                    std::move_iterator(int_middle),
                                    std::move_iterator(int_middle),
                                    std::move_iterator(int_end),
                                    out_begin,
                                    compare,
                                    swaps);
}

}

/** Splits the array into two parts. The first part is composed of all entries that
 * fulfil the predicate. Those that don't, are moved to the second part. The first
 * item of the second part is returned.
 */

template <typename Iterator, typename UnaryPredicate>
Iterator partition(Iterator begin, Iterator end, UnaryPredicate pred)
{
    auto partition_point = begin;
    for (; begin != end; std::advance(begin, 1)) {
        if (pred(*begin)) {
            std::iter_swap(partition_point, begin);
            std::advance(partition_point, 1);
        }
    }
    return partition_point;
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

template <typename LInputIterator, typename RInputIterator, typename OutputIterator, typename Comparator>
OutputIterator merge(LInputIterator left_begin,
                     LInputIterator left_end,
                     RInputIterator right_begin,
                     RInputIterator right_end,
                     OutputIterator out_begin,
                     Comparator compare)
{
    return internal::merge_impl<LInputIterator, RInputIterator, OutputIterator, Comparator>(
      left_begin, left_end, right_begin, right_end, out_begin, compare, nullptr);
}

template <typename InputIterator,
          typename OutputIterator,
          typename Comparator = std::less<typename std::iterator_traits<InputIterator>::value_type>>
OutputIterator merge_sort(InputIterator begin,
                          InputIterator end,
                          OutputIterator out_begin,
                          Comparator compare = Comparator{})
{
    return internal::merge_sort_impl<InputIterator, OutputIterator, Comparator, void>(
      begin, end, out_begin, compare, nullptr);
}

/// In-situ merge-sort + inversion count
template <typename InputIterator,
          typename OutputIterator,
          typename Comparator = std::less<typename std::iterator_traits<InputIterator>::value_type>>
std::size_t sort_and_count_inversions(InputIterator begin,
                                      InputIterator end,
                                      OutputIterator out_begin,
                                      Comparator compare = Comparator{})
{
    static_assert(std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category,
                                      std::bidirectional_iterator_tag>::value,
                  "InputIterator must be bidirectional");

    std::size_t swaps = 0;
    internal::merge_sort_impl<InputIterator, OutputIterator, Comparator, std::size_t>(
      begin, end, out_begin, compare, &swaps);
    return swaps;
}

/// In-situ, non-randomized quicksort
template <typename InputIterator,
          typename Comparator = std::less<typename std::iterator_traits<InputIterator>::value_type>>
void quick_sort(InputIterator begin, InputIterator end, Comparator compare = Comparator{})
{
    const auto size = std::distance(begin, end);
    if (size < internal::QUICKSORT_MIN_SIZE) {
        my::internal::non_recursive_sort<InputIterator, Comparator, void>(begin, end, compare, nullptr);
        return;
    }

    const auto pivot = *std::next(begin, size / 2);
    auto partition_point = std::partition(begin, end, [&](auto& x) -> bool { return compare(x, pivot); });

    quick_sort(begin, partition_point, compare);
    quick_sort(partition_point, end, compare);
}

}