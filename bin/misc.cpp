#include <inttypes.h>

#include "argparser/all.hpp"
#include "gtest/gtest.h"

TEST(ArgparserFlag, Default)
{
    int i = 0;
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

TEST(ArgparserFlag, FailedIfRequiredNotProvided)
{
    int required = 0;
    int optional = 0;
    auto parser = argparser::new_parser("parse int");
    EXPECT_TRUE(
        parser->flag(&required, "--required", "-r", "A required number"));
    EXPECT_TRUE(parser->flag(
        &optional, "--optional", "-o", "An optional number", "181"));
    const char *arg[] = {"./argtest", "--optional", "284"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}

TEST(ArgparserFlag, RequiredCanOverride)
{
    int required = 1;
    int optional = 0;
    auto parser = argparser::new_parser("parse int");
    EXPECT_TRUE(
        parser->flag(&required, "--required", "-r", "A required number"));
    EXPECT_TRUE(parser->flag(
        &optional, "--optional", "-o", "An optional number", "181"));
    const char *arg[] = {"./argtest", "--optional", "284", "--required", "2"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(required, 2);
}

TEST(ArgparserFlag, NotAllowDupFullFlag)
{
    int required = 1;
    int reason = 0;
    auto parser = argparser::new_parser("parse int");
    EXPECT_TRUE(parser->flag(&required, "--required", "-r", ""));
    EXPECT_FALSE(parser->flag(&reason, "--required", "-o", "A conflict flag"));
    const char *arg[] = {"./argtest", "--required", "2"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(required, 2);
}
TEST(ArgparserFlag, NotAllowDupShortFlag)
{
    int required = 1;
    int reason = 0;
    auto parser = argparser::new_parser("parse int");
    EXPECT_TRUE(parser->flag(&required, "--required", "-r", ""));
    EXPECT_FALSE(
        parser->flag(&reason, "--reason", "-r", "A conflict short flag"));
    const char *arg[] = {"./argtest", "-r", "2"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(required, 2);
    EXPECT_EQ(reason, 0);
}

TEST(ArgparserFlag, ConflictFlagNotRegistered)
{
    int required = 1;
    int reason = 0;
    auto parser = argparser::new_parser("parse int");
    EXPECT_TRUE(parser->flag(&required, "--required", "-r", ""));
    EXPECT_FALSE(parser->flag(&reason, "--required", "-o", "A conflict flag"));
    const char *arg[] = {"./argtest", "--required", "2", "-o", "8"};
    // should fail, -o not registered
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(reason, 0);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}