#include "gtest/gtest.h"
#include "argparser/all.hpp"
#include <inttypes.h>

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
    EXPECT_TRUE(parser->flag(&i, "--zero", "-z", "== 0"));
    EXPECT_TRUE(parser->flag(&i, "--positive", "-p", "> 0"));
    EXPECT_TRUE(parser->flag(&i, "--negative", "-n", "< 0"));
    const char* arg[] = {"./argtest", "--int", "5", "--zero", "0", "--positive", "8", "--negative=-21"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(i, 5);
    EXPECT_EQ(zero, 0);
    EXPECT_EQ(pos, 8);
    EXPECT_EQ(neg, -21);
}

TEST(ArgparserFlag, ParseInt64)
{
    int64_t i = 0;
    auto parser = argparser::new_parser("parse int");
    EXPECT_TRUE(parser->flag(&i, "--int", "-i", "number i"));
    const char* arg[] = {"./argtest", "--int", "5"};
    EXPECT_TRUE(parser->parse(3, arg));
    EXPECT_EQ(i, 5);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}