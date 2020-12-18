#include <inttypes.h>

#include <string>

#include "argparser/all.hpp"
#include "gtest/gtest.h"

TEST(ArgparserCommand, ShouldParseCommandFlag)
{
    int64_t i;
    uint64_t u;
    std::string s;
    double d;
    bool b = false;

    int64_t root_i;
    uint64_t root_u;
    std::string root_s;
    double root_d;
    bool root_b = false;

    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&root_i, "--i", "", "", "0"));
    EXPECT_TRUE(parser->flag(&root_u, "--u", "", "", "0"));
    EXPECT_TRUE(parser->flag(&root_s, "--s", "", "", "0"));
    EXPECT_TRUE(parser->flag(&root_d, "--d", "", "", "0"));
    EXPECT_TRUE(parser->flag(&root_b, "--b", "", "", "0"));

    auto &sub_parser = parser->command("test");
    EXPECT_TRUE(sub_parser.flag(&i, "--i", "", "", "0"));
    EXPECT_TRUE(sub_parser.flag(&u, "--u", "", "", "0"));
    EXPECT_TRUE(sub_parser.flag(&s, "--s", "", "", "0"));
    EXPECT_TRUE(sub_parser.flag(&d, "--d", "", "", "0"));
    EXPECT_TRUE(sub_parser.flag(&b, "--b", "", "", "0"));

    const char *arg[] = {"./argtest",
                         "test",
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
TEST(ArgparserCommand, ShouldParseCommandAllocatedFlag)
{
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag("--i", "", "", "0"));
    EXPECT_TRUE(parser->flag("--u", "", "", "0"));
    EXPECT_TRUE(parser->flag("--s", "", "", "0"));
    EXPECT_TRUE(parser->flag("--d", "", "", "0"));
    EXPECT_TRUE(parser->flag("--b", "", "", "0"));

    auto &sub_parser = parser->command("test");
    EXPECT_TRUE(sub_parser.flag("--i", "", "", "0"));
    EXPECT_TRUE(sub_parser.flag("--u", "", "", "0"));
    EXPECT_TRUE(sub_parser.flag("--s", "", "", "0"));
    EXPECT_TRUE(sub_parser.flag("--d", "", "", "0"));
    EXPECT_TRUE(sub_parser.flag("--b", "", "", "0"));

    const char *arg[] = {"./argtest",
                         "test",
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

    auto& store = parser->store();
    ASSERT_TRUE(store.has("--i"));
    ASSERT_TRUE(store.has("--u"));
    ASSERT_TRUE(store.has("--s"));
    ASSERT_TRUE(store.has("--d"));
    ASSERT_TRUE(store.has("--b"));
    EXPECT_EQ(store.get("--i").to<int64_t>(), 2);
    EXPECT_EQ(store.get("--u").to<uint64_t>(), 3);
    EXPECT_STREQ(store.get("--s").to<std::string>().c_str(), "say hi!");
    EXPECT_EQ(store.get("--d").to<double>(), -7.5);
    EXPECT_EQ(store.get("--b").to<bool>(), true);
}
TEST(ArgparserCommand, ShouldParseRootFlag)
{
    int64_t i;
    uint64_t u;
    std::string s;
    double d;
    bool b = false;

    int64_t root_i;
    uint64_t root_u;
    std::string root_s;
    double root_d;
    bool root_b = false;

    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&root_i, "--i", "", "", "0"));
    EXPECT_TRUE(parser->flag(&root_u, "--u", "", "", "0"));
    EXPECT_TRUE(parser->flag(&root_s, "--s", "", "", "0"));
    EXPECT_TRUE(parser->flag(&root_d, "--d", "", "", "0"));
    EXPECT_TRUE(parser->flag(&root_b, "--b", "", "", "0"));

    auto &sub_parser = parser->command("test");
    EXPECT_TRUE(sub_parser.flag(&i, "--i", "", "", "0"));
    EXPECT_TRUE(sub_parser.flag(&u, "--u", "", "", "0"));
    EXPECT_TRUE(sub_parser.flag(&s, "--s", "", "", "0"));
    EXPECT_TRUE(sub_parser.flag(&d, "--d", "", "", "0"));
    EXPECT_TRUE(sub_parser.flag(&b, "--b", "", "", "0"));

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
    EXPECT_EQ(root_i, 2);
    EXPECT_EQ(root_u, 3);
    EXPECT_STREQ(root_s.c_str(), "say hi!");
    EXPECT_EQ(root_d, -7.5);
    EXPECT_EQ(root_b, true);
}
TEST(ArgparserCommand, ShouldFollowLongFlag)
{
    int64_t i;
    auto parser = argparser::new_parser();
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
    auto &sub_parser_1 = parser->command("same");
    auto &sub_parser_2 = parser->command("same");
    EXPECT_TRUE(sub_parser_1.flag(&i, "--i", "", "", "0"));
    EXPECT_TRUE(sub_parser_2.flag(&j, "--j", "", "", "0"));

    const char *arg[] = {"./argtest", "same", "--i=12345", "--j=23456"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(i, 12345);
    EXPECT_EQ(j, 23456);
}
TEST(ArgparserCommand, RootCommandNotAcceptSubFlag)
{
    int64_t i;
    uint64_t j;
    auto parser = argparser::new_parser();
    auto &sub_parser = parser->command("same");
    EXPECT_TRUE(parser->flag(&i, "--i", "", "", "0"));
    EXPECT_TRUE(sub_parser.flag(&j, "--j", "", "", "0"));

    const char *arg[] = {"./argtest", "--j=23456"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}
TEST(ArgparserCommand, RootCommandOnlyRequireRootFlag)
{
    int64_t i;
    uint64_t j;
    auto parser = argparser::new_parser();
    auto &sub_parser = parser->command("same");
    EXPECT_TRUE(parser->flag(&i, "--i", "", ""));
    EXPECT_TRUE(sub_parser.flag(&j, "--j", "", ""));

    const char *arg[] = {"./argtest", "--i=12345"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(i, 12345);
}
TEST(ArgparserCommand, FailedIfRootCommandWithSubFlag)
{
    int64_t i;
    uint64_t j;
    auto parser = argparser::new_parser();
    auto &sub_parser = parser->command("same");
    EXPECT_TRUE(parser->flag(&i, "--i", "", ""));
    EXPECT_TRUE(sub_parser.flag(&j, "--j", "", ""));

    const char *arg[] = {"./argtest", "--j=12345"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}
TEST(ArgparserCommand, SubCommandNotAcceptRootFlag)
{
    int64_t i;
    uint64_t j;
    auto parser = argparser::new_parser();
    auto &sub_parser = parser->command("same");
    EXPECT_TRUE(parser->flag(&i, "--i", "", "", "0"));
    EXPECT_TRUE(sub_parser.flag(&j, "--j", "", "", "0"));

    const char *arg[] = {"./argtest", "same", "--i=12345"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}
TEST(ArgparserCommand, SubCommandOnlyRequireSubFlag)
{
    int64_t i;
    uint64_t j;
    auto parser = argparser::new_parser();
    auto &sub_parser = parser->command("same");
    EXPECT_TRUE(parser->flag(&i, "--i", "", ""));
    EXPECT_TRUE(sub_parser.flag(&j, "--j", "", ""));

    const char *arg[] = {"./argtest", "same", "--j=23456"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(j, 23456);
}
TEST(ArgparserCommand, FailedIfSubCommandWithRootFlag)
{
    int64_t i;
    uint64_t j;
    auto parser = argparser::new_parser();
    auto &sub_parser = parser->command("same");
    EXPECT_TRUE(parser->flag(&i, "--i", "", ""));
    EXPECT_TRUE(sub_parser.flag(&j, "--j", "", ""));

    const char *arg[] = {"./argtest", "same", "--i=12345"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}