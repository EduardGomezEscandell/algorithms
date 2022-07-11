#pragma once

#include <doctest/doctest.h>
#include "include/algorithms.hpp"

#include <vector>
#include <list>
#include <forward_list>

TEST_CASE_TEMPLATE("algortihms.hpp", TContainer, std::vector<int>, std::list<int>, std::forward_list<int>)
{
    SUBCASE("sorted_partition_point")
    {
        constexpr std::size_t arrsize = my::internal::BINSEARCH_MIN_SIZE * 2.1;
        std::array<int, arrsize> arr{};
        std::iota(arr.begin(), arr.end(), 0);

        TContainer a(arr.cbegin(), arr.cend());

        const auto it = my::sorted_partition_point(std::cbegin(a), std::cend(a), [](auto x) { return x < 4; });
        const auto dist = std::distance(std::cbegin(a), it);
        CHECK_EQ(dist, 4);

        const auto it2 = my::sorted_partition_point(std::cbegin(a), std::cend(a),
                                                    [](auto x) { return static_cast<std::size_t>(x) < (arrsize * 2); });
        CHECK_EQ(it2, std::cend(a));

        TContainer b(arr.crbegin(), arr.crend());
        const auto it3 = my::sorted_partition_point(std::cbegin(b), std::cend(b),
                                                    [](auto x) { return x > my::internal::BINSEARCH_MIN_SIZE; });
        const auto dist3 = std::distance(std::cbegin(b), it3);
        CHECK_EQ(dist3, arrsize - my::internal::BINSEARCH_MIN_SIZE - 1);

        const TContainer c{};
        const auto it4 = my::sorted_partition_point(std::cbegin(c), std::cend(c), [](auto x) { return x > 3; });
        CHECK_EQ(it4, std::cend(c));
    }

    SUBCASE("selection_sort")
    {
        TContainer a{1, 8, 9, 13, 6, 13, 10, 13, 0, 18};
        const TContainer asc{0, 1, 6, 8, 9, 10, 13, 13, 13, 18};
        const TContainer des{18, 13, 13, 13, 10, 9, 8, 6, 1, 0};

        my::selection_sort(std::begin(a), std::end(a));
        CHECK_EQ(a, asc);

        my::selection_sort(std::begin(a), std::end(a), std::greater<>{});
        CHECK_EQ(a, des);

        TContainer d{};
        my::selection_sort(std::begin(d), std::end(d));
        CHECK_EQ(d, TContainer{});
    }

    SUBCASE("merge_sort")
    {
        constexpr std::size_t arrsize = my::internal::MERGESORT_MIN_SIZE * 2.1;
        std::array<int, arrsize> arr{};
        std::iota(arr.begin(), arr.end(), 0);

        TContainer asc(arr.cbegin(), arr.cend());
        TContainer des(arr.crbegin(), arr.crend());
        TContainer des2(des);

        // Ascending
        my::merge_sort(std::begin(des2), std::end(des2));
        CHECK_EQ(asc, des2);

        // Descending
        my::merge_sort(std::begin(asc), std::end(asc), std::greater<int>{});
        CHECK_EQ(asc, des);

        // Empty
        TContainer c{};
        my::merge_sort(std::begin(c), std::end(c));
        CHECK_EQ(c, TContainer{});
    }

    SUBCASE("sort_and_count_inversions")
    {
        if constexpr (std::is_convertible<
                        typename std::iterator_traits<typename TContainer::iterator>::iterator_category,
                        std::bidirectional_iterator_tag>::value) {

            constexpr std::size_t arrsize = my::internal::MERGESORT_MIN_SIZE * 2.1;
            std::array<int, arrsize> arr{};
            std::iota(arr.begin(), arr.end(), 0);

            TContainer asc(arr.cbegin(), arr.cend());
            TContainer des(arr.crbegin(), arr.crend());

            // Empty
            {
                TContainer c{};
                auto invs = my::sort_and_count_inversions(std::begin(c), std::end(c));
                CHECK_EQ(invs, 0);
            }
            // Small array
            {
                TContainer c{1, 3, 5, 2, 4, 6};
                auto invs = my::sort_and_count_inversions(std::begin(c), std::end(c));
                CHECK_EQ(invs, 3);
            }

            // Ascending-Ascending
            {
                auto copy = asc;
                auto invs = my::sort_and_count_inversions(std::begin(copy), std::end(copy));
                CHECK_EQ(invs, 0);
            }

            // Descending-Ascending
            {
                auto copy = asc;
                auto invs = my::sort_and_count_inversions(std::begin(copy), std::end(copy), std::greater<int>{});
                constexpr auto expected = arrsize * (arrsize - 1) / 2;
                CHECK_EQ(invs, expected);
            }

            // Ascending-Descending
            {
                auto copy = des;
                auto invs = my::sort_and_count_inversions(std::begin(copy), std::end(copy));
                constexpr auto expected = arrsize * (arrsize - 1) / 2;
                CHECK_EQ(invs, expected);
            }
            // Descending-Descending
            {
                auto copy = des;
                auto invs = my::sort_and_count_inversions(std::begin(copy), std::end(copy), std::greater<int>{});
                CHECK_EQ(invs, 0);
            }
        }
    }
}