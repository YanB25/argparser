#include <inttypes.h>

#include <string>

#include "argparser/all.hpp"
#include "gtest/gtest.h"

TEST(ArgparserParseGlobalFlag, ShouldParseGlobalFlag)
{
    int64_t i;
    uint64_t u;
    std::string s;
    double d;
    bool b = false;

    int64_t g_i;
    uint64_t g_u;
    std::string g_s;
    double g_d;
    bool g_b = false;

    auto parser = argparser::new_parser();
    // failed to register, full-name flag does not follow --flag-name form
    EXPECT_TRUE(parser->global_flag(&g_i, "--gi", "", "", "0"));
    EXPECT_TRUE(parser->global_flag(&g_u, "--gu", "", "", "0"));
    EXPECT_TRUE(parser->global_flag(&g_s, "--gs", "", "", "0"));
    EXPECT_TRUE(parser->global_flag(&g_d, "--gd", "", "", "0"));
    EXPECT_TRUE(parser->global_flag(&g_b, "--gb", "", "", "0"));

    EXPECT_TRUE(parser->flag(&i, "--i", "", "", "0"));
    EXPECT_TRUE(parser->flag(&u, "--u", "", "", "0"));
    EXPECT_TRUE(parser->flag(&s, "--s", "", "", "0"));
    EXPECT_TRUE(parser->flag(&d, "--d", "", "", "0"));
    EXPECT_TRUE(parser->flag(&b, "--b", "", "", "0"));

    const char *arg[] = {"./argtest",
                         "--gi",
                         "2",
                         "--gu",
                         "3",
                         "--gs",
                         "say hi!",
                         "--gd",
                         "-7.5",
                         "--gb",
                         "true"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(g_i, 2);
    EXPECT_EQ(g_u, 3);
    EXPECT_STREQ(g_s.c_str(), "say hi!");
    EXPECT_EQ(g_d, -7.5);
    EXPECT_EQ(g_b, true);
}
TEST(ArgparserCommand, ShouldParseNonGlobalFlag)
{
    int64_t i;
    uint64_t u;
    std::string s;
    double d;
    bool b = false;

    int64_t g_i;
    uint64_t g_u;
    std::string g_s;
    double g_d;
    bool g_b = false;

    auto parser = argparser::new_parser();
    // failed to register, full-name flag does not follow --flag-name form
    EXPECT_TRUE(parser->global_flag(&g_i, "--gi", "", "", "0"));
    EXPECT_TRUE(parser->global_flag(&g_u, "--gu", "", "", "0"));
    EXPECT_TRUE(parser->global_flag(&g_s, "--gs", "", "", "0"));
    EXPECT_TRUE(parser->global_flag(&g_d, "--gd", "", "", "0"));
    EXPECT_TRUE(parser->global_flag(&g_b, "--gb", "", "", "0"));

    EXPECT_TRUE(parser->flag(&i, "--i", "", "", "0"));
    EXPECT_TRUE(parser->flag(&u, "--u", "", "", "0"));
    EXPECT_TRUE(parser->flag(&s, "--s", "", "", "0"));
    EXPECT_TRUE(parser->flag(&d, "--d", "", "", "0"));
    EXPECT_TRUE(parser->flag(&b, "--b", "", "", "0"));

    const char *arg[] = {"./argtest",
                         "--i",
                         "2",
                         "--u",
                         "3",
                         "--s",
                         "say hi!",
                         "--d",
                         "-7.5",
                         "--b",
                         "true"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(i, 2);
    EXPECT_EQ(u, 3);
    EXPECT_STREQ(s.c_str(), "say hi!");
    EXPECT_EQ(d, -7.5);
    EXPECT_EQ(b, true);
}
TEST(ArgparserCommand, ShouldFollowLongFlag)
{
    int64_t i;
    auto parser = argparser::new_parser();
    // failed to register, full-name flag does not follow --flag-name form
    auto &sub_parser_1 = parser->command("one");
    auto &sub_parser_2 = sub_parser_1.command("two");
    auto &sub_parser_3 = sub_parser_2.command("three");
    auto &sub_parser_4 = sub_parser_3.command("four");
    auto &sub_parser_5 = sub_parser_4.command("five");
    auto &sub_parser_6 = sub_parser_5.command("six");
    EXPECT_TRUE(sub_parser_6.flag(&i, "--i", "", "", "0"));

    const char *arg[] = {
        "./argtest", "one", "two", "three", "four", "five", "six", "--i=12345"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(i, 12345);
}

TEST(ArgparserCommand, SameCommandSameParser)
{
    int64_t i;
    uint64_t j;
    auto parser = argparser::new_parser();
    // failed to register, full-name flag does not follow --flag-name form
    auto &sub_parser_1 = parser->command("same");
    auto &sub_parser_2 = parser->command("same");
    EXPECT_TRUE(sub_parser_1.flag(&i, "--i", "", "", "0"));
    EXPECT_TRUE(sub_parser_2.flag(&j, "--j", "", "", "0"));

    const char *arg[] = {
        "./argtest", "same", "--i=12345", "--j=23456"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(i, 12345);
    EXPECT_EQ(j, 23456);
}
TEST(ArgparserCommand, RootCommandNotAcceptSubFlag)
{
    int64_t i;
    uint64_t j;
    auto parser = argparser::new_parser();
    // failed to register, full-name flag does not follow --flag-name form
    auto &sub_parser = parser->command("same");
    EXPECT_TRUE(parser->flag(&i, "--i", "", "", "0"));
    EXPECT_TRUE(sub_parser.flag(&j, "--j", "", "", "0"));

    const char *arg[] = {
        "./argtest", "--j=23456"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}
TEST(ArgparserCommand, RootCommandOnlyRequireRootFlag)
{
    int64_t i;
    uint64_t j;
    auto parser = argparser::new_parser();
    // failed to register, full-name flag does not follow --flag-name form
    auto &sub_parser = parser->command("same");
    EXPECT_TRUE(parser->flag(&i, "--i", "", ""));
    EXPECT_TRUE(sub_parser.flag(&j, "--j", "", ""));

    const char *arg[] = {
        "./argtest", "--i=12345"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(i, 12345);
}
TEST(ArgparserCommand, FailedIfRootCommandWithSubFlag)
{
    int64_t i;
    uint64_t j;
    auto parser = argparser::new_parser();
    // failed to register, full-name flag does not follow --flag-name form
    auto &sub_parser = parser->command("same");
    EXPECT_TRUE(parser->flag(&i, "--i", "", ""));
    EXPECT_TRUE(sub_parser.flag(&j, "--j", "", ""));

    const char *arg[] = {
        "./argtest", "--j=12345"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}
TEST(ArgparserCommand, SubCommandNotAcceptRootFlag)
{
    int64_t i;
    uint64_t j;
    auto parser = argparser::new_parser();
    // failed to register, full-name flag does not follow --flag-name form
    auto &sub_parser = parser->command("same");
    EXPECT_TRUE(parser->flag(&i, "--i", "", "", "0"));
    EXPECT_TRUE(sub_parser.flag(&j, "--j", "", "", "0"));

    const char *arg[] = {
        "./argtest", "same", "--i=12345"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}
TEST(ArgparserCommand, SubCommandOnlyRequireSubFlag)
{
    int64_t i;
    uint64_t j;
    auto parser = argparser::new_parser();
    // failed to register, full-name flag does not follow --flag-name form
    auto &sub_parser = parser->command("same");
    EXPECT_TRUE(parser->flag(&i, "--i", "", ""));
    EXPECT_TRUE(sub_parser.flag(&j, "--j", "", ""));

    const char *arg[] = {
        "./argtest", "same", "--j=23456"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(j, 23456);
}
TEST(ArgparserCommand, FailedIfSubCommandWithRootFlag)
{
    int64_t i;
    uint64_t j;
    auto parser = argparser::new_parser();
    // failed to register, full-name flag does not follow --flag-name form
    auto &sub_parser = parser->command("same");
    EXPECT_TRUE(parser->flag(&i, "--i", "", ""));
    EXPECT_TRUE(sub_parser.flag(&j, "--j", "", ""));

    const char *arg[] = {
        "./argtest", "same", "--i=12345"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}