#include <inttypes.h>

#include "argparser/all.hpp"
#include "gtest/gtest.h"

TEST(ArgparserFlag, Default)
{
    int i= 0;
    unsigned int u = 0;
    int64_t ii = 0;
    uint64_t uu = 0;
    bool t = false;
    bool f = true;
    auto parser = argparser::new_parser("parse int");
    EXPECT_TRUE(parser->flag(&i, "--i-val", "", "", "-123"));
    EXPECT_TRUE(parser->flag(&u, "--u-val", "", "", "456"));
    EXPECT_TRUE(parser->flag(&ii, "--ii-val", "", "", "-789"));
    EXPECT_TRUE(parser->flag(&uu, "--uu-val", "", "", "101112"));
    EXPECT_TRUE(parser->flag(&t, "--t-val", "", "", "true"));
    EXPECT_TRUE(parser->flag(&f, "--f-val", "", "", "false"));
    const char *arg[] = {"./argtest"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(i, -123);
    EXPECT_EQ(u, 456);
    EXPECT_EQ(ii, -789);
    EXPECT_EQ(uu, 101112);
    EXPECT_EQ(t, true);
    EXPECT_EQ(f, false);
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}