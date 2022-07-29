#pragma once

#include <array>
#include <functional>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <cassert>
#include <random>

namespace my
{

namespace internal
{

struct AlgoConfig
{
    static std::ptrdiff_t MERGESORT_MIN_SIZE;
    static std::ptrdiff_t BINSEARCH_MIN_SIZE;
    static std::ptrdiff_t QUICKSORT_MIN_SIZE;

    enum QuicksortPivotChoice
    {
        FIRST,
        LAST,
        SAMPLE_3,
        RANDOM
    };

    static QuicksortPivotChoice QUICKSORT_PIVOT_CHOICE;
};

inline std::ptrdiff_t AlgoConfig::MERGESORT_MIN_SIZE = 100;
inline std::ptrdiff_t AlgoConfig::BINSEARCH_MIN_SIZE = 100;
inline std::ptrdiff_t AlgoConfig::QUICKSORT_MIN_SIZE = 100;
inline auto AlgoConfig::QUICKSORT_PIVOT_CHOICE = AlgoConfig::QuicksortPivotChoice::RANDOM;

/// Component of insertion sort. Takes a sorted range [first, last) rotates moves the 'last' element to the right place.
template <typename BiderectionalIterator, typename BinaryPredicate, typename SwapCounter>
BiderectionalIterator sort_last_forward(BiderectionalIterator first,
                                        BiderectionalIterator last,
                                        BinaryPredicate comp,
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
template <typename BiderectionalIterator, typename BinaryPredicate, typename SwapCounter>
void insertion_sort_impl(BiderectionalIterator begin,
                         BiderectionalIterator end,
                         BinaryPredicate comp,
                         SwapCounter* swaps)
{
    auto begin_unsorted = begin;
    while (begin_unsorted != end) {
        std::advance(begin_unsorted, 1);
        sort_last_forward<BiderectionalIterator, BinaryPredicate, SwapCounter>(begin, begin_unsorted, comp, swaps);
    }
}

/// In-situ selection-sort
template <typename InputIterator, typename BinaryPredicate>
void selection_sort_impl(InputIterator begin, InputIterator end, BinaryPredicate compare)
{
    for (; begin != end; std::advance(begin, 1)) {
        auto min_elem = std::min_element(begin, end, compare);
        std::iter_swap(begin, min_elem);
    }
}

// Non-recursive search algorithm metaprogramming
template <typename Iterator, typename BinaryPredicate, typename SwapCounter>
void non_recursive_sort(Iterator begin, Iterator end, BinaryPredicate compare, SwapCounter* swaps)
{
    if constexpr (std::is_convertible<typename std::iterator_traits<Iterator>::iterator_category,
                                      std::bidirectional_iterator_tag>::value) {
        insertion_sort_impl<Iterator, BinaryPredicate, SwapCounter>(begin, end, compare, swaps);
    } else {
        static_assert(std::is_same_v<SwapCounter, void>,
                      "Swap counter is not supported for non-bidirectional iterators");
        selection_sort_impl<Iterator, BinaryPredicate>(begin, end, compare);
    }
}

}

template <typename InputIterator, typename UnaryPredicate>
InputIterator sorted_partition_point(InputIterator begin, InputIterator end, UnaryPredicate pred)
{
    const auto size = std::distance(begin, end);
    if (size < internal::AlgoConfig::BINSEARCH_MIN_SIZE) {
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

template <typename LInputIterator, typename RInputIterator, typename OutputIterator, typename BinaryPredicate,
          typename SwapCounter = void>
OutputIterator merge_impl(LInputIterator left_begin,
                          LInputIterator left_end,
                          RInputIterator right_begin,
                          RInputIterator right_end,
                          OutputIterator out_begin,
                          BinaryPredicate compare,
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
template <typename InputIterator, typename OutputIterator, typename BinaryPredicate, typename SwapCounter>
OutputIterator merge_sort_impl(InputIterator begin,
                               InputIterator end,
                               OutputIterator out_begin,
                               BinaryPredicate compare,
                               SwapCounter* swaps)
{
    const auto size = std::distance(begin, end);

    if (size < AlgoConfig::MERGESORT_MIN_SIZE) {
        auto out_end = std::copy(begin, end, out_begin);
        non_recursive_sort<OutputIterator, BinaryPredicate, SwapCounter>(out_begin, out_end, compare, swaps);
        return out_end;
    }

    auto middle = std::next(begin, size / 2);

    auto intermediate_container = std::vector(begin, end);
    auto int_begin = intermediate_container.begin();
    auto int_middle = std::next(int_begin, size / 2);
    auto int_end = intermediate_container.end();

    merge_sort_impl<InputIterator, decltype(int_begin), BinaryPredicate, SwapCounter>(begin, middle, int_begin, compare,
                                                                                      swaps);
    merge_sort_impl<InputIterator, decltype(int_begin), BinaryPredicate, SwapCounter>(middle, end, int_middle, compare,
                                                                                      swaps);

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
    if (begin == end) {
        return begin;
    }
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
          typename BinaryPredicate = std::less<typename std::iterator_traits<InputIterator>::value_type>>
void insertion_sort(InputIterator begin, InputIterator end, BinaryPredicate compare = BinaryPredicate{})
{
    internal::insertion_sort_impl<InputIterator, BinaryPredicate, void>(begin, end, compare, nullptr);
}

template <typename InputIterator,
          typename BinaryPredicate = std::less<typename std::iterator_traits<InputIterator>::value_type>>
void selection_sort(InputIterator begin, InputIterator end, BinaryPredicate compare = BinaryPredicate{})
{
    internal::selection_sort_impl<InputIterator, BinaryPredicate>(begin, end, compare);
}

template <typename LInputIterator, typename RInputIterator, typename OutputIterator, typename BinaryPredicate>
OutputIterator merge(LInputIterator left_begin,
                     LInputIterator left_end,
                     RInputIterator right_begin,
                     RInputIterator right_end,
                     OutputIterator out_begin,
                     BinaryPredicate compare)
{
    return internal::merge_impl<LInputIterator, RInputIterator, OutputIterator, BinaryPredicate>(
      left_begin, left_end, right_begin, right_end, out_begin, compare, nullptr);
}

template <typename InputIterator, typename OutputIterator,
          typename BinaryPredicate = std::less<typename std::iterator_traits<InputIterator>::value_type>>
OutputIterator merge_sort(InputIterator begin,
                          InputIterator end,
                          OutputIterator out_begin,
                          BinaryPredicate compare = BinaryPredicate{})
{
    return internal::merge_sort_impl<InputIterator, OutputIterator, BinaryPredicate, void>(begin, end, out_begin,
                                                                                           compare, nullptr);
}

/// In-situ merge-sort + inversion count
template <typename InputIterator, typename OutputIterator,
          typename BinaryPredicate = std::less<typename std::iterator_traits<InputIterator>::value_type>>
std::size_t sort_and_count_inversions(InputIterator begin,
                                      InputIterator end,
                                      OutputIterator out_begin,
                                      BinaryPredicate compare = BinaryPredicate{})
{
    static_assert(std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category,
                                      std::bidirectional_iterator_tag>::value,
                  "InputIterator must be bidirectional");

    std::size_t swaps = 0;
    internal::merge_sort_impl<InputIterator, OutputIterator, BinaryPredicate, std::size_t>(begin, end, out_begin,
                                                                                           compare, &swaps);
    return swaps;
}

namespace internal
{

template <typename ForwardIterator, typename BinaryPredicate>
ForwardIterator quick_sort_choose_pivot(ForwardIterator begin,
                                        ForwardIterator end,
                                        [[maybe_unused]] BinaryPredicate&& compare)
{
    assert(begin != end);

    switch (AlgoConfig::QUICKSORT_PIVOT_CHOICE) {
    case AlgoConfig::QuicksortPivotChoice::FIRST:
        return begin;
    case AlgoConfig::QuicksortPivotChoice::LAST: {
        const auto size = std::distance(begin, end);
        return std::next(begin, size - 1);
    }
    case AlgoConfig::QuicksortPivotChoice::SAMPLE_3: {
        const auto size = std::distance(begin, end);
        const auto midpoint = (size - 1) / 2;

        auto midpoint_it = std::next(begin, midpoint);
        auto last_it = std::next(midpoint_it, size - midpoint - 1);
        std::array<ForwardIterator, 3> arr{begin, midpoint_it, last_it};

        auto iter_comp = [&](const auto& it1, const auto& it2) { return compare(*it1, *it2); };
        non_recursive_sort<typename std::array<ForwardIterator, 3>::iterator, decltype(iter_comp), void>(
          arr.begin(), arr.end(), iter_comp, nullptr);
        return arr[1];
    }
    case AlgoConfig::QuicksortPivotChoice::RANDOM: {
        const auto size = std::distance(begin, end);

        std::uniform_int_distribution<std::size_t> distrib(0, size - 1);
        std::random_device rd;
        std::mt19937 gen(rd());
        return std::next(begin, distrib(gen));
    }
    }
}

/*
  Preconditions:
    - `it` and `anchor` are in the same array
    - `anchor` preceedes `it` (corollary: `it` is not the first element of the array)

  Postconditions:
    - returns the element preceding `it` in the array.

  Complexity
    Given the distance between `anchor` and `it`, named `n`:
    - O(1) for bidirectional and random access iterators.
    - O(n) for forward iterators.

  Advise
    Using `begin` as the anchor is the most general choice.
*/
template <typename Iterator>
Iterator prev(Iterator it, [[maybe_unused]] Iterator anchor)
{
    if constexpr (std::is_convertible<typename std::iterator_traits<Iterator>::iterator_category,
                                      std::bidirectional_iterator_tag>::value) {
        return std::prev(it);
    } else {
        auto prev = anchor;
        while (anchor != it) {
            prev = std::exchange(anchor, std::next(anchor)); // same as prev=anchor++
        }
        return prev;
    }
}
} // namespace internal

#ifdef QUICKSORT_COUNT_COMPARISONS
inline std::size_t n_comparisons;
#endif

/// In-situ, non-randomized quicksort
template <typename InputIterator,
          typename BinaryPredicate = std::less<typename std::iterator_traits<InputIterator>::value_type>>
void quick_sort(InputIterator begin, InputIterator end, BinaryPredicate compare = BinaryPredicate{})
{
    if (begin == end) {
        return;
    }
    const auto size = std::distance(begin, end);

#ifdef QUICKSORT_COUNT_COMPARISONS
    n_comparisons += size - 1;
#endif

    if (size < internal::AlgoConfig::QUICKSORT_MIN_SIZE) {
        my::internal::non_recursive_sort<InputIterator, BinaryPredicate, void>(begin, end, compare, nullptr);
        return;
    }

    auto pivot = internal::quick_sort_choose_pivot(begin, end, compare);

    // Copying pivot value to begin
    std::iter_swap(pivot, begin);
    pivot = std::exchange(begin, std::next(begin));

    auto partition_point = my::partition(begin, end, [&](const auto& x) -> bool { return compare(x, *pivot); });

    // Sort left side, if it exists
    if (partition_point != begin) {
        auto last_left = internal::prev(partition_point, begin);
        std::iter_swap(pivot, last_left);
        begin = std::exchange(pivot, last_left);
        quick_sort(begin, pivot, compare);
    }
    quick_sort(partition_point, end, compare);
}

/**
 * Finds the @n th value in sorted order in an unsorted range [ @begin , @end ), according to ordering defined by
 * binary predicate @compare.
 */
template <typename InputIterator,
          typename BinaryPredicate = std::less<typename std::iterator_traits<InputIterator>::value_type>>
InputIterator nth_element(InputIterator begin,
                          InputIterator end,
                          std::size_t n,
                          BinaryPredicate compare = BinaryPredicate{})
{
    std::size_t size = std::distance(begin, end);
    assert(n < size);

    if (size == 1) {
        return begin;
    }

    auto pivot = internal::quick_sort_choose_pivot(begin, end, compare);
    std::iter_swap(pivot, begin);
    pivot = std::exchange(begin, std::next(begin));

    auto partition_point = my::partition(begin, end, [&](const auto& x) -> bool { return compare(x, *pivot); });
    std::size_t pivot_pos = std::distance(begin, partition_point);

    if (pivot_pos > n) {
        return nth_element(begin, partition_point, n, compare);
    }
    if (pivot_pos < n) {
        return nth_element(partition_point, end, n - pivot_pos - 1, compare);
    }

    return pivot;
}

/* Divides a sequence according to a predicate, and reduces the intervals.
 * For instance, to sum a comma-separated list of numbers
 * batch_reduce(string.cbegin()
 *              , string.cend()
 *              , equal_to(,)
 *              , [](auto begin, auto end) { return begin==end ? 0 : atoi(&*begin); }
 *              , std::plus<int>{});
 */
template <typename ForwardIterator, typename Value, typename UnaryPredicate, typename Transform, typename Reduce>
auto batch_transform_reduce(ForwardIterator begin, ForwardIterator end, Value value, UnaryPredicate&& Pred,
                            Transform&& transform, Reduce&& reduce)
{
    auto it = begin;
    while (it != end) {
        it = std::find_if(begin, end, Pred);
        value = reduce(std::forward<Value>(value), transform(begin, it));
        begin = std::next(it);
    }
    return value;
}

} // namespace my
