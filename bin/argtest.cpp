#include <inttypes.h>

#include "argparser/all.hpp"
#include "gtest/gtest.h"

TEST(FactorialTest, HandlesZeroInput)
{
    EXPECT_EQ(1, 1);
}

TEST(ArgparserFlag, Trivial)
{
    auto parser = argparser::new_parser("trivial");
}

TEST(ArgparserFlag, ParseInt)
{
    int i = 0;
    int zero = -1;
    int pos = -1;
    int neg = 1;
    auto parser = argparser::new_parser("parse int");
    EXPECT_TRUE(parser->flag(&i, "--int", "-i", "number i"));
    EXPECT_TRUE(parser->flag(&zero, "--zero", "-z", "== 0"));
    EXPECT_TRUE(parser->flag(&pos, "--positive", "-p", "> 0"));
    EXPECT_TRUE(parser->flag(&neg, "--negative", "-n", "< 0"));
    const char *arg[] = {"./argtest",
                         "--int",
                         "5",
                         "--zero",
                         "0",
                         "--positive",
                         "8",
                         "--negative",
                         "-21"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(i, 5);
    EXPECT_EQ(zero, 0);
    EXPECT_EQ(pos, 8);
    EXPECT_EQ(neg, -21);
}

TEST(ArgparserFlag, ParseBigInt)
{
    int64_t big_int;
    int64_t big_int_p_1;
    int64_t big_int_m_1;

    // a negative number -2^{63}
    const int64_t expect = 1ull << 63;
    // expect + 1
    int64_t expect_p1 = (1ull << 63);
    expect_p1 += 1;
    // expect 3 1
    int64_t expect_m1 = (1ull << 63);
    expect_m1 += 3;

    auto parser = argparser::new_parser("parse int");
    EXPECT_TRUE(parser->flag(&big_int, "--big-int", "-b", "big number"));
    EXPECT_TRUE(
        parser->flag(&big_int_p_1, "--big-int-p-1", "-p", "big number + 1"));
    EXPECT_TRUE(
        parser->flag(&big_int_m_1, "--big-int-m-1", "-m", "big number - 1"));
    const char *arg[] = {"./argtest",
                         "--big-int",
                         "-9223372036854775808",
                         "--big-int-p-1",
                         "-9223372036854775807",
                         "--big-int-m-1",
                         "-9223372036854775805"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(big_int, expect);
    EXPECT_EQ(big_int_p_1, expect_p1);
    EXPECT_EQ(big_int_m_1, expect_m1);
}

TEST(ArgparserFlag, ParseInt64)
{
    int64_t i = 0;
    auto parser = argparser::new_parser("parse int");
    EXPECT_TRUE(parser->flag(&i, "--int", "-i", "number i"));
    const char *arg[] = {"./argtest", "--int", "5"};
    EXPECT_TRUE(parser->parse(3, arg));
    EXPECT_EQ(i, 5);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}