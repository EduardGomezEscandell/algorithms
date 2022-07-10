#pragma once

#include <doctest/doctest.h>
#include "include/algorithms.hpp"

#include <vector>
#include <list>

TEST_CASE_TEMPLATE("algortihms.hpp", TContainer, std::vector<int>, std::list<int>)
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

    SUBCASE("insertion_sort")
    {
        TContainer a{1, 8, 9, 13, 6, 13, 10, 13, 0, 18};
        const TContainer b{0, 1, 6, 8, 9, 10, 13, 13, 13, 18};
        const TContainer c(std::crbegin(b), std::crend(b));

        my::insertion_sort(std::begin(a), std::end(a));
        CHECK_EQ(a, b);

        my::insertion_sort(std::begin(a), std::end(a), std::greater<>{});
        CHECK_EQ(a, c);

        TContainer d{};
        my::insertion_sort(std::begin(d), std::end(d));
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
}