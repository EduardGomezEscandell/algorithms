#pragma once
#include <doctest/doctest.h>
#include "include/bigint.hpp"

TEST_CASE_TEMPLATE("bigint.hpp", WORD, std::uint8_t, unsigned short, unsigned long)
{
    using INT = my::big_uint<WORD>;

    SUBCASE("IO")
    {
        const INT a{"0"};
        CHECK_EQ(a.to_string(), "0x0");

        const INT b{"123"};
        CHECK_EQ(b.to_string(), "0x7b");

        const INT c{"256"};
        CHECK_EQ(c.to_string(), "0x100");

        const INT d{"3457"};
        CHECK_EQ(d.to_string(), "0xd81");

        const INT e{"8092539"};
        CHECK_EQ(e.to_string(), "0x7b7b7b");

        const INT f{"63216312623"};
        CHECK_EQ(f.to_string(), "0xeb7fc612f");

        const INT g{"345421345531"};
        CHECK_EQ(g.to_string(), "0x506cb772fb");

        const INT h{"408637658154"};
        CHECK_EQ(h.to_string(), "0x5f24b3d42a");
    }
    SUBCASE("Addition")
    {
        const INT a1{"10"};
        const INT b1{"20"};
        const INT c1{"30"};
        CHECK_EQ(a1 + b1, c1);

        const INT a2{"0"};
        const INT b2{"0"};
        const INT expected2{"0"};
        CHECK_EQ(a2 + b2, expected2);

        const INT a3{"255"};
        const INT b3{"1"};
        const INT expected3{"256"};
        CHECK_EQ(a3 + b3, expected3);

        const INT a4{"623"};
        const INT b4{"531"};
        const INT expected4{"1154"};
        CHECK_EQ(a4 + b4, expected4);

        const INT a5{"63216312623"};
        const INT b5{"345421345531"};
        const INT expected{"408637658154"};
        CHECK_EQ(a5 + b5, expected);

        const INT a6{"12345678965216435645"};
        const INT b6{"683411624631216543135135123"};
        const INT expected6{"683411636976895508351570768"};
        CHECK_EQ(a6 + b6, expected6);
    }
    SUBCASE("Subtraction")
    {
        const INT a1{"0"};
        const INT b1{"0"};
        const INT expected1{"0"};
        CHECK_EQ(a1 - b1, expected1);

        const INT a2{"10"};
        const INT b2{"10"};
        const INT expected2{"0"};
        CHECK_EQ(a2 - b2, expected2);

        const INT a3{"13"};
        const INT b3{"5"};
        const INT expected3{"8"};
        CHECK_EQ(a3 - b3, expected3);

        const INT a4{"260"};
        const INT b4{"10"};
        const INT expected4{"250"};
        CHECK_EQ(a4 - b4, expected4);

        const INT a5{"6531654653"};
        const INT b5{"6500000000"};
        const INT expected5{"31654653"};
        CHECK_EQ(a5 - b5, expected5);

        const INT a6{"683411636976895508351570768"};
        const INT b6{"683411624631216543135135123"};
        const INT expected6{"12345678965216435645"};
        CHECK_EQ(a6 - b6, expected6);
    }
    SUBCASE("Product")
    {
        const INT a1{"0"};
        const INT b1{"0"};
        const INT expected1{"0"};
        CHECK_EQ(a1 * b1, expected1);

        const INT a2{"7"};
        const INT b2{"3"};
        const INT expected2{"21"};
        CHECK_EQ(a2 * b2, expected2);

        const INT a3{"200"};
        const INT b3{"10"};
        const INT expected3{"2000"};
        CHECK_EQ(a3 * b3, expected3);

        const INT a4{"256"};
        const INT b4{"256"};
        const INT expected4{"65536"};
        CHECK_EQ(a4 * b4, expected4);

        const INT a5{"5678"};
        const INT b5{"1069"};
        const INT expected5{"6069782"};
        CHECK_EQ(a5 * b5, expected5);

        const INT a6{"1234"};
        const INT b6{"5678"};
        const INT expected6{"7006652"};
        CHECK_EQ(a6 * b6, expected6);

        const INT a7{"6538645315"};
        const INT b7{"96543684210"};
        const INT expected7{"631264908452555976150"};
        CHECK_EQ(a7 * b7, expected7);

        const INT a8{"6538645315"};
        const INT b8{"96543684210"};
        const INT expected8{"631264908452555976150"};
        CHECK_EQ(a8 * b8, expected8);

        const INT a9{"3141592653589793238462643383279502884197169399375105820974944592"};
        const INT b9{"2718281828459045235360287471352662497757247093699959574966967627"};
        const INT expected9{"8539734222673567065463550869546574495034888535765114961879601127067743"
                            "044893204848617875072216249073013374895871952806582723184"};
        CHECK_EQ(a9 * b9, expected9);
    }
}
