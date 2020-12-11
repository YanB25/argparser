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
TEST(ArgparserFlag, EmptyShortOrFullIsAllow)
{
    int required = 1;
    int reason = 0;
    int a = 0;
    int b = 0;
    auto parser = argparser::new_parser("parse int");
    EXPECT_TRUE(parser->flag(&required, "--required", "", ""));
    EXPECT_TRUE(parser->flag(&reason, "--reason", "", ""));
    EXPECT_TRUE(parser->flag(&a, "", "-a", ""));
    EXPECT_TRUE(parser->flag(&b, "", "-b", ""));
    const char *arg[] = {"./argtest",
                         "--required",
                         "2",
                         "--reason",
                         "8",
                         "-a",
                         "-5",
                         "-b",
                         "-10"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(required, 2);
    EXPECT_EQ(reason, 8);
    EXPECT_EQ(a, -5);
    EXPECT_EQ(b, -10);
}
TEST(ArgparserFlag, BothEmptyNotAllowed)
{
    int required = 1;
    auto parser = argparser::new_parser("parse int");
    EXPECT_FALSE(parser->flag(&required, "", "", ""));
    const char *arg[] = {"./argtest"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(required, 1);
}
TEST(ArgparserFlag, SpaceIsEmptyAndIsNotAllowed)
{
    int a = 1;
    int b = 1;
    int c = 1;
    auto parser = argparser::new_parser("parse int");
    EXPECT_FALSE(parser->flag(&a, " ", "", ""));
    EXPECT_FALSE(parser->flag(&b, "", " ", ""));
    EXPECT_FALSE(parser->flag(&c, " ", " ", ""));
    const char *arg[] = {"./argtest"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 1);
    EXPECT_EQ(c, 1);
}
TEST(ArgparserFlag, StringCanContainSpace)
{
    std::string name;
    const char* content = "Hello world! It's a special long sentence.";
    auto parser = argparser::new_parser("parse int");
    EXPECT_TRUE(parser->flag(&name, "--name", "-n", "The name"));
    const char *arg[] = {"./argtest", "--name", content};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_STREQ(name.c_str(), content);
}
TEST(ArgparserFlag, CharStarNotAnArray)
{
    char buffer[1024] = {};
    const char* content = "Hello world! It's a special long sentence.";
    auto parser = argparser::new_parser("parse int");
    EXPECT_TRUE(parser->flag(buffer, "--name", "-n", "The name"));
    const char *arg[] = {"./argtest", "--name", content};
    parser->parse(sizeof(arg) / sizeof(arg[0]), arg);
    // we expect that only the first char is modified
    // because `buffer` is char*.
    for (size_t i = 1; i < 1024; ++i)
    {
        EXPECT_EQ(buffer[i], '\0');
    }
}

TEST(ArgparserFlag, SpaceNotAllowIfItsNotString)
{
    int64_t i = 0;
    const char* str_i = "-123 456";
    auto parser = argparser::new_parser("");
    EXPECT_TRUE(parser->flag(&i, "--i", "", ""));
    const char *arg[] = {"./argtest", "--i", str_i};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}
TEST(ArgparserFlag, SpaceNotAllowIfItsNotString2)
{
    uint64_t u = 0;
    const char* str_u = "123 456";
    auto parser = argparser::new_parser("");
    EXPECT_TRUE(parser->flag(&u, "--u", "", ""));
    const char *arg[] = {"./argtest", "--u", str_u};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}
TEST(ArgparserFlag, SpaceNotAllowIfItsNotString3)
{
    bool b = false;
    const char* str_b = "true 1";
    auto parser = argparser::new_parser("");
    EXPECT_TRUE(parser->flag(&b, "--b", "", ""));
    const char *arg[] = {"./argtest", "--b", str_b};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}
TEST(ArgparserFlag, SpaceNotAllowIfItsNotString4)
{
    double d = 1;
    const char* str_d = "1.4234 5";
    auto parser = argparser::new_parser("");
    EXPECT_TRUE(parser->flag(&d, "--d", "", ""));
    const char *arg[] = {"./argtest", "--d", str_d};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}
TEST(ArgparserFlag, FailedIfFlagProvidedTwice)
{
    int64_t flag = 1;
    auto parser = argparser::new_parser("");
    EXPECT_TRUE(parser->flag(&flag, "--f", "", ""));
    const char *arg[] = {"./argtest", "--f", "5", "--f", "6"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}