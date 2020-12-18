#include <inttypes.h>

#include <string>

#include "argparser/argparser.hpp"
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
TEST(ArgparserCommand, ShouldConflit)
{
    int64_t i;
    uint64_t j;
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&i, "--i", "-j", "", "0"));
    EXPECT_FALSE(parser->global_flag(&j, "--i", "-k", "", "0"));
}
TEST(ArgparserCommand, ShouldConflit2)
{
    int64_t i;
    uint64_t j;
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&i, "--i", "-k", "", "0"));
    EXPECT_FALSE(parser->global_flag(&j, "--j", "-k", "", "0"));
}

TEST(ArgparserCommand, GlobalIsAlwaysGlobal)
{
    int64_t i;
    int64_t trivial;
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->global_flag(&i, "--i", "-k", "", "0"));
    auto &sub_parser_1 = parser->command("one");
    auto &sub_parser_2 = sub_parser_1.command("two");
    auto &sub_parser_3 = sub_parser_2.command("three");
    auto &sub_parser_4 = sub_parser_3.command("four");
    auto &sub_parser_5 = sub_parser_4.command("five");
    auto &sub_parser_6 = sub_parser_5.command("six");
    EXPECT_TRUE(sub_parser_6.global_flag(&trivial, "--trivial", "", "", "0"));
    const char *arg[] = {
        "./argtest", "one", "two", "three", "four", "five", "six", "--i=12345"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(i, 12345);
}
TEST(ArgparserCommand, GlobalIsAlwaysGlobal2)
{
    int64_t i;
    int64_t trivial;
    auto parser = argparser::new_parser();
    auto &sub_parser_1 = parser->command("one");
    auto &sub_parser_2 = sub_parser_1.command("two");
    auto &sub_parser_3 = sub_parser_2.command("three");
    auto &sub_parser_4 = sub_parser_3.command("four");
    auto &sub_parser_5 = sub_parser_4.command("five");
    auto &sub_parser_6 = sub_parser_5.command("six");
    EXPECT_TRUE(sub_parser_3.global_flag(&i, "--i", "-k", "", "0"));
    EXPECT_TRUE(sub_parser_6.global_flag(&trivial, "--trivial", "", "", "0"));
    const char *arg[] = {
        "./argtest", "one", "two", "three", "four", "five", "six", "--i=12345"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(i, 12345);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}