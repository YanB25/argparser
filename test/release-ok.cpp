#include <inttypes.h>

// include the release header.
#include "../release/argparser.hpp"
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
    auto parser = argparser::new_parser();
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
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&i, "--int", "-i", "number i"));
    EXPECT_TRUE(parser->flag(&i2, "--int2", "-j", "number i2"));
    const char *arg[] = {"./argtest", "--int", "5", "--int2", "-5"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(i, 5);
    EXPECT_EQ(i2, -5);
}

TEST(ArgparserFlag, ParseInt64ShouldFailForChar)
{
    int64_t i = 0;
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&i, "--int", "-i", "number i"));
    const char *arg[] = {"./argtest", "--int", "5abc"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}

TEST(ArgparserFlag, ParseInt64ShouldFailForSpace)
{
    int64_t i = 0;
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&i, "--int", "-i", "number i"));
    const char *arg[] = {"./argtest", "--int", "5 abc"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}

TEST(ArgparserFlag, ParseInt64ShouldFailForSpace2)
{
    int64_t i = 0;
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&i, "--int", "-i", "number i"));
    const char *arg[] = {"./argtest", "--int", "5 10"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
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

    auto parser = argparser::new_parser();
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

    auto parser = argparser::new_parser();
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
    auto parser = argparser::new_parser();
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
    auto parser = argparser::new_parser();
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

TEST(ArgparserFlag, ParseArray)
{
    std::vector<int64_t> arrs;
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&arrs, "--array", "-a", ""));
    const char *arg[] = {"./argtest", "--array", "1,2,3,4,5,6,7,8,9,10"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(arrs.size(), 10);
    for (int i = 0; i < 10; ++i)
    {
        EXPECT_EQ(arrs[i], i + 1);
    }
}

TEST(ArgparserFlag, ParseStringArray)
{
    std::vector<std::string> arrs;
    auto parser = argparser::new_parser();
    std::vector<std::string> expected = {
        "hello", "world", "my", "name", "is", "LiHua."};
    EXPECT_TRUE(parser->flag(&arrs, "--array", "-a", ""));
    const char *arg[] = {
        "./argtest", "--array", "hello,world,my,name,is,LiHua."};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    ASSERT_EQ(arrs.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i)
    {
        EXPECT_EQ(arrs[i], expected[i]);
    }
}

TEST(ArgparserFlag, ParseStringWithComma)
{
    std::string str;
    std::string expected = "hello,world,my,name,is,LiHua.";
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&str, "--str", "-s", ""));
    const char *arg[] = {"./argtest", "--str", expected.c_str()};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_STREQ(str.c_str(), expected.c_str());
}

TEST(ArgparserFlag, ParseAnyArray)
{
    std::vector<int64_t> iarrs;
    std::vector<uint64_t> uarrs;
    std::vector<double> darrs;
    std::vector<bool> barrs;
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&iarrs, "--array-i", "-i", ""));
    EXPECT_TRUE(parser->flag(&uarrs, "--array-u", "-u", ""));
    EXPECT_TRUE(parser->flag(&darrs, "--array-d", "-d", ""));
    EXPECT_TRUE(parser->flag(&barrs, "--array-b", "-b", ""));
    const char *arg[] = {"./argtest",
                         "-i",
                         "1,-2,3,-4,5,-6,7,-8,9,-10",
                         "-u",
                         "1,2,3,4,5",
                         "-d",
                         "1.2,-3.4,5.6,-7.8",
                         "-b",
                         "true,false,1,0"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(iarrs.size(), 10);
    for (int i = 0; i < 10; ++i)
    {
        EXPECT_EQ(iarrs[i], i % 2 == 0 ? i + 1 : -i - 1);
    }
    EXPECT_EQ(uarrs.size(), 5);
    for (int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(uarrs[i], i + 1);
    }
    EXPECT_EQ(darrs.size(), 4);
    EXPECT_EQ(darrs[0], 1.2);
    EXPECT_EQ(darrs[1], -3.4);
    EXPECT_EQ(darrs[2], 5.6);
    EXPECT_EQ(darrs[3], -7.8);
    EXPECT_EQ(barrs.size(), 4);
    EXPECT_EQ(barrs[0], true);
    EXPECT_EQ(barrs[1], false);
    EXPECT_EQ(barrs[2], true);
    EXPECT_EQ(barrs[3], false);
}

TEST(ArgparserFlag, ParseArrayFailedAtMiddle)
{
    std::vector<int64_t> array;
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(
        &array, "--array", "-a", "An array that has wrong middle elements"));
    const char *arg[] = {"./argtest", "-a", "1,2,t,4"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}
TEST(ArgparserFlag, ParseArrayFailedAtMiddle2)
{
    std::vector<bool> array;
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(
        &array, "--array", "-a", "An array that has wrong middle elements"));
    // 2 is also not valid
    const char *arg[] = {"./argtest", "-a", "true,false,2,0"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}

TEST(ArgparserFlag, Default)
{
    int i = 0;
    unsigned int u = 0;
    int64_t ii = 0;
    uint64_t uu = 0;
    bool t = false;
    bool f = true;
    auto parser = argparser::new_parser();
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

TEST(ArgparserFlag, FailedIfRequiredFlagsNotProvided)
{
    int required = 0;
    int optional = 0;
    auto parser = argparser::new_parser();
    EXPECT_TRUE(
        parser->flag(&required, "--required", "-r", "A required number"));
    EXPECT_TRUE(parser->flag(
        &optional, "--optional", "-o", "An optional number", "181"));
    const char *arg[] = {"./argtest", "--optional", "284"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}

TEST(ArgparserFlag, CanOverrideDefaultValue)
{
    int required = 1;
    int optional = 0;
    auto parser = argparser::new_parser();
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
    auto parser = argparser::new_parser();
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
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&required, "--required", "-r", ""));
    EXPECT_FALSE(
        parser->flag(&reason, "--reason", "-r", "A conflict short flag"));
    const char *arg[] = {"./argtest", "-r", "2"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_EQ(required, 2);
    EXPECT_EQ(reason, 0);
}

TEST(ArgparserFlag, ConflictFlagShouldNotBeRegistered)
{
    int required = 1;
    int reason = 0;
    auto parser = argparser::new_parser();
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
    auto parser = argparser::new_parser();
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
TEST(ArgparserFlag, BothShortAndFullEmptyIsNotAllowed)
{
    int required = 1;
    auto parser = argparser::new_parser();
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
    auto parser = argparser::new_parser();
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
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&name, "--name", "-n", "The name"));
    const char *arg[] = {"./argtest", "--name", content};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_STREQ(name.c_str(), content);
}
TEST(ArgparserFlag, CharPointerIsNotAnArray)
{
    char buffer[1024] = {};
    const char* content = "Hello world! It's a special long sentence.";
    auto parser = argparser::new_parser();
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

TEST(ArgparserFlag, SpaceNotAllowIfItIsNotString)
{
    int64_t i = 0;
    const char* str_i = "-123 456";
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&i, "--i", "", ""));
    const char *arg[] = {"./argtest", "--i", str_i};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}
TEST(ArgparserFlag, SpaceNotAllowIfItsNotString2)
{
    uint64_t u = 0;
    const char* str_u = "123 456";
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&u, "--u", "", ""));
    const char *arg[] = {"./argtest", "--u", str_u};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}
TEST(ArgparserFlag, SpaceNotAllowIfItsNotString3)
{
    bool b = false;
    const char* str_b = "true 1";
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&b, "--b", "", ""));
    const char *arg[] = {"./argtest", "--b", str_b};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}
TEST(ArgparserFlag, SpaceNotAllowIfItsNotString4)
{
    double d = 1;
    const char* str_d = "1.4234 5";
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&d, "--d", "", ""));
    const char *arg[] = {"./argtest", "--d", str_d};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}
TEST(ArgparserFlag, FailedIfFlagProvidedTwice)
{
    int64_t flag = 1;
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&flag, "--f", "", ""));
    const char *arg[] = {"./argtest", "--f", "5", "--f", "6"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}
TEST(ArgparserFlag, FlagThatFailedToRegisterShouldNotExists)
{
    int64_t flag = 1;
    auto parser = argparser::new_parser();
    // failed to register, default value "" not parsable
    EXPECT_FALSE(parser->flag(&flag, "--f", "", "", ""));
    const char *arg[] = {"./argtest", "--f", "5"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}
TEST(ArgparserFlag, FlagThatFailedToRegisterShouldNotExists2)
{
    int64_t flag = 1;
    auto parser = argparser::new_parser();
    // failed to register, default value "" not parsable
    EXPECT_FALSE(parser->flag(&flag, "--f", "", "", ""));
    const char *arg[] = {"./argtest"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}
TEST(ArgparserFlag, FlagThatFailedToRegisterShouldNotExists3)
{
    int64_t flag = 1;
    auto parser = argparser::new_parser();
    // failed to register, full-name flag does not follow --flag-name form
    EXPECT_FALSE(parser->flag(&flag, "abc", "", ""));
    const char *arg[] = {"./argtest"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}
TEST(ArgparserFlag, FlagThatFailedToRegisterShouldNotExists4)
{
    int64_t flag = 1;
    auto parser = argparser::new_parser();
    // failed to register, full-name flag does not follow --flag-name form
    EXPECT_FALSE(parser->flag(&flag, "abc", "", ""));
    const char *arg[] = {"./argtest", "abc", "4"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}