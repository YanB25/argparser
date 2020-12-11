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

TEST(ArgparserFlag, ParseInt64)
{
    int64_t i = 0;
    int64_t i2 = 0;
    auto parser = argparser::new_parser("parse int");
    EXPECT_TRUE(parser->flag(&i, "--int", "-i", "number i"));
    EXPECT_TRUE(parser->flag(&i2, "--int2", "-j", "number i2"));
    const char *arg[] = {"./argtest", "--int", "5", "--int2", "-5"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(i, 5);
    EXPECT_EQ(i2, -5);
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

    auto int_str = std::to_string(expect);
    auto int_p1_str = std::to_string(expect_p1);
    auto int_m1_str = std::to_string(expect_m1);

    auto parser = argparser::new_parser("parse int");
    EXPECT_TRUE(parser->flag(&big_int, "--big-int", "-b", "big number"));
    EXPECT_TRUE(
        parser->flag(&big_int_p_1, "--big-int-p-1", "-p", "big number + 1"));
    EXPECT_TRUE(
        parser->flag(&big_int_m_1, "--big-int-m-1", "-m", "big number - 1"));
    const char *arg[] = {"./argtest",
                         "--big-int",
                         int_str.c_str(),
                         "--big-int-p-1",
                         int_p1_str.c_str(),
                         "--big-int-m-1",
                         int_m1_str.c_str()};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(big_int, expect);
    EXPECT_EQ(big_int_p_1, expect_p1);
    EXPECT_EQ(big_int_m_1, expect_m1);
}

TEST(ArgparserFlag, ParseUnInt)
{
    uint32_t ui;
    uint64_t u_big_i;

    uint32_t expect_ui = 1 << 30;
    uint64_t expect_u_big_i = 1ull << 60;

    auto expect_str = std::to_string(expect_ui);
    auto expect_u_big_i_str = std::to_string(expect_u_big_i);

    auto parser = argparser::new_parser("parse unint");
    EXPECT_TRUE(parser->flag(&ui, "--unsigned-int", "-u", "unsigned number"));
    EXPECT_TRUE(parser->flag(
        &u_big_i, "--unsigned-big-int", "-b", "unsigned big number"));
    const char *arg[] = {"./argtest",
                         "--unsigned-int",
                         expect_str.c_str(),
                         "--unsigned-big-int",
                         expect_u_big_i_str.c_str()};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(ui, expect_ui);
    EXPECT_EQ(u_big_i, expect_u_big_i);
}

TEST(ArgparserFlag, ParseBool)
{
    bool t = false;
    bool one = false;
    bool f = true;
    bool zero = true;
    auto parser = argparser::new_parser("parse bool");
    EXPECT_TRUE(parser->flag(&t, "--tr", "", ""));
    EXPECT_TRUE(parser->flag(&one, "--one", "", ""));
    EXPECT_TRUE(parser->flag(&f, "--fa", "", ""));
    EXPECT_TRUE(parser->flag(&zero, "--zero", "", ""));
    const char *arg[] = {"./argtest",
                         "--tr",
                         "true",
                         "--fa",
                         "false",
                         "--one",
                         "1",
                         "--zero",
                         "0"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(t, true);
    EXPECT_EQ(one, true);
    EXPECT_EQ(f, false);
    EXPECT_EQ(zero, false);
}
TEST(ArgparserFlag, ParseDouble)
{
    double d1;
    double d2;
    double d3;
    double d4;
    auto parser = argparser::new_parser("parse bool");
    EXPECT_TRUE(parser->flag(&d1, "--d1", "", ""));
    EXPECT_TRUE(parser->flag(&d2, "--d2", "", ""));
    EXPECT_TRUE(parser->flag(&d3, "--d3", "", ""));
    EXPECT_TRUE(parser->flag(&d4, "--d4", "", ""));
    const char *arg[] = {"./argtest",
                         "--d1",
                         "314.564",
                         "--d2",
                         "-423.523",
                         "--d3",
                         "1e8",
                         "--d4",
                         "-1e6"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_DOUBLE_EQ(d1, 314.564);
    EXPECT_DOUBLE_EQ(d2, -423.523);
    EXPECT_DOUBLE_EQ(d3, 1e8);
    EXPECT_DOUBLE_EQ(d4, -1e6);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}