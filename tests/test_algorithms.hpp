#pragma once

#include <doctest/doctest.h>

#include "include/algorithms.hpp"

#include <vector>
#include <list>
#include <forward_list>
#include <execution>

TEST_CASE_TEMPLATE("algortihms.hpp", TContainer, std::vector<int>, std::list<int>, std::forward_list<int>)
{
    SUBCASE("sorted_partition_point")
    {
        // Empty case
        const TContainer c{};
        const auto it0 = my::sorted_partition_point(std::cbegin(c), std::cend(c), [](auto x) { return x > 3; });
        CHECK_EQ(it0, std::cend(c));

        const std::size_t arrsize = my::internal::AlgoConfig::BINSEARCH_MIN_SIZE * 2.1;
        std::vector<int> arr(arrsize, 0);
        std::iota(arr.begin(), arr.end(), 0);

        TContainer data(arr.cbegin(), arr.cend());

        // Left side
        const auto it1 = my::sorted_partition_point(std::cbegin(data), std::cend(data), [](auto x) { return x < 4; });
        const auto dist = std::distance(std::cbegin(data), it1);
        CHECK_EQ(dist, 4);

        // Right side
        const auto it2 = my::sorted_partition_point(std::cbegin(data), std::cend(data),
                                                    [&](auto x) { return x < std::ptrdiff_t(arrsize) - 10; });
        const auto dist2 = std::distance(it2, std::cend(data));
        CHECK_EQ(dist2, 10);

        // Reverse
        TContainer b(arr.crbegin(), arr.crend());
        const auto it3 = my::sorted_partition_point(
          std::cbegin(b), std::cend(b), [](auto x) { return x > my::internal::AlgoConfig::BINSEARCH_MIN_SIZE; });
        const auto dist3 = std::distance(std::cbegin(b), it3);
        CHECK_EQ(dist3, arrsize - my::internal::AlgoConfig::BINSEARCH_MIN_SIZE - 1);

        // Not found
        const auto it4 = my::sorted_partition_point(
          std::cbegin(data), std::cend(data), [&](auto x) { return static_cast<std::size_t>(x) < (arrsize * 2); });
        CHECK_EQ(it4, std::cend(data));
    }

    SUBCASE("selection_sort")
    {
        TContainer empty_c{};
        my::selection_sort(std::begin(empty_c), std::end(empty_c));

        TContainer a{1, 8, 9, 13, 6, 13, 10, 13, 0, 18};
        const TContainer asc{0, 1, 6, 8, 9, 10, 13, 13, 13, 18};
        const TContainer des{18, 13, 13, 13, 10, 9, 8, 6, 1, 0};

        my::selection_sort(std::begin(a), std::end(a));
        CHECK_EQ(a, asc);

        my::selection_sort(std::begin(a), std::end(a), std::greater<>{});
        CHECK_EQ(a, des);
    }

    SUBCASE("insertion_sort")
    {
        // Skipping for forward-iterable-only containers
        if constexpr (std::is_convertible<
                        typename std::iterator_traits<typename TContainer::iterator>::iterator_category,
                        std::bidirectional_iterator_tag>::value) {
            TContainer empty_c{};
            my::insertion_sort(std::begin(empty_c), std::end(empty_c));
            CHECK_EQ(empty_c, TContainer{});

            TContainer a{1, 8, 9, 13, 6, 13, 10, 13, 0, 18};
            const TContainer asc{0, 1, 6, 8, 9, 10, 13, 13, 13, 18};
            const TContainer des{18, 13, 13, 13, 10, 9, 8, 6, 1, 0};

            my::insertion_sort(std::begin(a), std::end(a));
            CHECK_EQ(a, asc);

            my::insertion_sort(std::begin(a), std::end(a), std::greater<>{});
            CHECK_EQ(a, des);
        }
    }

    SUBCASE("merge_sort")
    {
        const std::size_t arrsize = my::internal::AlgoConfig::MERGESORT_MIN_SIZE * 2.1;
        std::vector<int> arr(arrsize, 0);
        std::iota(arr.begin(), arr.end(), 0);

        TContainer asc(arr.cbegin(), arr.cend());
        TContainer des(arr.crbegin(), arr.crend());

        // Ascending
        auto outp = TContainer(arrsize, {});
        my::merge_sort(std::cbegin(des), std::cend(des), std::begin(outp));
        CHECK_EQ(asc, outp);

        // Descending
        std::for_each(std::execution::par_unseq, std::begin(outp), std::end(outp), [](auto& x) { x = {}; });
        my::merge_sort(std::cbegin(asc), std::cend(asc), std::begin(outp), std::greater<int>{});
        CHECK_EQ(des, outp);

        // Empty
        outp = TContainer{};
        TContainer c{};
        my::merge_sort(std::begin(c), std::end(c), std::begin(outp));
        CHECK_EQ(outp, TContainer{});
    }

    SUBCASE("quick_sort")
    {
        const std::size_t arrsize = my::internal::AlgoConfig::QUICKSORT_MIN_SIZE * 2.1;
        std::vector<int> arr(arrsize, 0);
        std::iota(arr.begin(), arr.end(), 0);

        // Empty
        TContainer c{};
        my::quick_sort(std::begin(c), std::end(c));
        CHECK_EQ(c, TContainer{});

        // Small
        TContainer a{3, 8, 2, 5, 1, 4, 7, 6};
        my::quick_sort(std::begin(a), std::end(a));
        CHECK_EQ(a, TContainer{1, 2, 3, 4, 5, 6, 7, 8});

        TContainer asc(arr.cbegin(), arr.cend());
        TContainer des(arr.crbegin(), arr.crend());
        TContainer des2(des);

        // Ascending
        my::quick_sort(std::begin(des2), std::end(des2));
        CHECK_EQ(asc, des2);

        // Descending
        my::quick_sort(std::begin(asc), std::end(asc), std::greater<int>{});
        CHECK_EQ(asc, des);
    }

    SUBCASE("sort_and_count_inversions")
    {
        if constexpr (std::is_convertible<
                        typename std::iterator_traits<typename TContainer::iterator>::iterator_category,
                        std::bidirectional_iterator_tag>::value) {

            const std::size_t arrsize = my::internal::AlgoConfig::MERGESORT_MIN_SIZE * 2.1;
            std::vector<int> arr(arrsize, 0);
            std::iota(arr.begin(), arr.end(), 0);

            TContainer asc(arr.cbegin(), arr.cend());
            TContainer des(arr.crbegin(), arr.crend());

            // Empty
            {
                TContainer outp{};
                TContainer c{};
                auto invs = my::sort_and_count_inversions(std::cbegin(c), std::cend(c), std::begin(outp));
                CHECK_EQ(invs, 0);
            }
            // Small array
            {
                TContainer c{1, 3, 5, 2, 4, 6};
                TContainer outp{c};
                auto invs = my::sort_and_count_inversions(std::cbegin(c), std::cend(c), std::begin(outp));
                CHECK_EQ(invs, 3);
            }

            // Ascending-Ascending
            {
                TContainer outp{asc};
                auto invs = my::sort_and_count_inversions(std::cbegin(asc), std::cend(asc), std::begin(outp));
                CHECK_EQ(invs, 0);
            }

            // Descending-Ascending
            {
                TContainer outp{asc};
                auto invs = my::sort_and_count_inversions(std::cbegin(asc), std::cend(asc), std::begin(outp),
                                                          std::greater<int>{});
                const auto expected = arrsize * (arrsize - 1) / 2;
                CHECK_EQ(invs, expected);
            }

            // Ascending-Descending
            {
                TContainer outp{des};
                auto invs = my::sort_and_count_inversions(std::cbegin(des), std::cend(des), std::begin(outp));
                const auto expected = arrsize * (arrsize - 1) / 2;
                CHECK_EQ(invs, expected);
            }
            // Descending-Descending
            {
                TContainer outp{des};
                auto invs = my::sort_and_count_inversions(std::cbegin(des), std::cend(des), std::begin(outp),
                                                          std::greater<int>{});
                CHECK_EQ(invs, 0);
            }
        }
    }

    SUBCASE("partition")
    {
        // Empty
        {
            TContainer c{};
            auto pp = my::partition(std::begin(c), std::end(c), [](auto x) { return x < 4; });
            CHECK_EQ(c, TContainer{});
            CHECK_EQ(pp, std::end(c));
        }
        // Small
        {
            TContainer c{1, 2, 3, 4, 5, 6, 7};
            auto pp = my::partition(std::begin(c), std::end(c), [](auto x) { return x % 2 != 0; });

            CHECK_EQ(std::distance(std::begin(c), pp), 4);
            std::for_each(std::begin(c), pp, [](auto x) { CHECK(x % 2 != 0); });
            std::for_each(pp, std::end(c), [](auto x) { CHECK(x % 2 == 0); });
        }
        // Big
        {
            constexpr auto size = 101;
            TContainer c(size, {});
            std::iota(c.begin(), c.end(), 0);
            auto pp = my::partition(std::begin(c), std::end(c), [](auto x) { return x % 2 != 0; });

            CHECK_EQ(std::distance(std::begin(c), pp), size / 2);
            std::for_each(std::begin(c), pp, [](auto x) { CHECK(x % 2 != 0); });
            std::for_each(pp, std::end(c), [](auto x) { CHECK(x % 2 == 0); });
        }
    }

    SUBCASE("nth_element")
    {
        const std::size_t arrsize = my::internal::AlgoConfig::QUICKSORT_MIN_SIZE * 2.1;
        std::vector<int> arr(arrsize, 0);
        std::iota(arr.begin(), arr.end(), 0);

        // Trivial
        {
            TContainer c{1};
            auto it = my::nth_element(std::begin(c), std::end(c), 0);
            REQUIRE_EQ(it, std::begin(c));
        }

        // LT
        {
            TContainer a{4, 1, 3, 6, 2, 5};
            for (int i = 0; i < 6; ++i) {
                auto it = my::nth_element(std::begin(a), std::end(a), i);
                CHECK_EQ(*it, i + 1);
            }
        }

        // GT
        {
            TContainer a{4, 1, 3, 6, 2, 5};
            for (int i = 0; i < 6; ++i) {
                auto it = my::nth_element(std::begin(a), std::end(a), i, std::greater<int>{});
                CHECK_EQ(*it, 6 - i);
            }
        }
    }
}