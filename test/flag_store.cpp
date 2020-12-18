#include <inttypes.h>

#include "argparser/all.hpp"
#include "gtest/gtest.h"

// TODO: consider allocated flag conflict with global flag/ normal flag
// TODO: consider parser->has() works well

TEST(FactorialTest, HandlesZeroInput)
{
    EXPECT_EQ(1, 1);
}

TEST(ArgparserFlagStore, Trivial)
{
    auto parser = argparser::new_parser("trivial");
}

TEST(ArgparserFlagStore, ParseInt)
{
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag("--int", "-i", "number i"));
    EXPECT_TRUE(parser->flag("--zero", "-z", "== 0"));
    EXPECT_TRUE(parser->flag("--positive", "-p", "> 0"));
    EXPECT_TRUE(parser->flag("--negative", "-n", "< 0"));
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
    auto &pi = parser->get("--int");
    auto &pz = parser->get("--zero");
    auto &pp = parser->get("--positive");
    auto &pn = parser->get("--negative");
    EXPECT_STREQ(pi.inner().c_str(), "5");
    EXPECT_EQ(pi.to<int>(), 5);
    EXPECT_STREQ(pz.inner().c_str(), "0");
    EXPECT_EQ(pz.to<int>(), 0);
    EXPECT_STREQ(pp.inner().c_str(), "8");
    EXPECT_EQ(pp.to<int>(), 8);
    EXPECT_STREQ(pn.inner().c_str(), "-21");
    EXPECT_EQ(pn.to<int>(), -21);
}

TEST(ArgparserFlagStore, ParseInt64)
{
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag("--int", "-i", "number i"));
    EXPECT_TRUE(parser->flag("--int2", "-j", "number i2"));
    const char *arg[] = {"./argtest", "--int", "5", "--int2", "-5"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    auto i = parser->get("--int");
    auto i2 = parser->get("--int2");
    EXPECT_STREQ(i.inner().c_str(), "5");
    EXPECT_EQ(i.to<int64_t>(), 5);
    EXPECT_STREQ(i2.inner().c_str(), "-5");
    EXPECT_EQ(i2.to<int64_t>(), -5);
}

TEST(ArgparserFlagStore, ParseInt64ShouldFailForChar)
{
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag("--int", "-i", "number i"));
    const char *arg[] = {"./argtest", "--int", "5abc"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_TRUE(parser->has("--int"));
    EXPECT_TRUE(parser->has("-i"));
    auto i = parser->get("--int");
    EXPECT_FALSE(i.convertable_to<int64_t>());
}

TEST(ArgparserFlagStore, ParseInt64ShouldFailForSpace)
{
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag("--int", "-i", "number i"));
    const char *arg[] = {"./argtest", "--int", "5 abc"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_TRUE(parser->has("--int"));
    EXPECT_TRUE(parser->has("-i"));
    auto i = parser->get("--int");

    EXPECT_FALSE(i.convertable_to<int64_t>());
}

TEST(ArgparserFlagStore, ParseInt64ShouldFailForSpace2)
{
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag("--int", "-i", "number i"));
    const char *arg[] = {"./argtest", "--int", "5 10"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_TRUE(parser->has("--int"));
    EXPECT_TRUE(parser->has("-i"));
    auto i = parser->get("--int");

    EXPECT_FALSE(i.convertable_to<int64_t>());
}

TEST(ArgparserFlagStore, ParseBigInt)
{
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
    EXPECT_TRUE(parser->flag("--big-int", "-b", "big number"));
    EXPECT_TRUE(parser->flag("--big-int-p-1", "-p", "big number + 1"));
    EXPECT_TRUE(parser->flag("--big-int-m-1", "-m", "big number - 1"));
    const char *arg[] = {"./argtest",
                         "--big-int",
                         int_str.c_str(),
                         "--big-int-p-1",
                         int_p1_str.c_str(),
                         "--big-int-m-1",
                         int_m1_str.c_str()};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    auto big_i = parser->get("--big-int");
    auto big_pi = parser->get("--big-int-p-1");
    auto big_mi = parser->get("--big-int-m-1");
    EXPECT_EQ(big_i.to<int64_t>(), expect);
    EXPECT_EQ(big_pi.to<int64_t>(), expect_p1);
    EXPECT_EQ(big_mi.to<int64_t>(), expect_m1);
    EXPECT_STREQ(big_i.inner().c_str(), std::to_string(expect).c_str());
    EXPECT_STREQ(big_pi.inner().c_str(), std::to_string(expect_p1).c_str());
    EXPECT_STREQ(big_mi.inner().c_str(), std::to_string(expect_m1).c_str());
}

TEST(ArgparserFlagStore, ParseUnInt)
{
    uint32_t expect_ui = 1 << 30;
    uint64_t expect_u_big_i = 1ull << 60;

    auto expect_str = std::to_string(expect_ui);
    auto expect_u_big_i_str = std::to_string(expect_u_big_i);

    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag("--unsigned-int", "-u", "unsigned number"));
    EXPECT_TRUE(
        parser->flag("--unsigned-big-int", "-b", "unsigned big number"));
    const char *arg[] = {"./argtest",
                         "--unsigned-int",
                         expect_str.c_str(),
                         "--unsigned-big-int",
                         expect_u_big_i_str.c_str()};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    auto u_i = parser->get("-u");
    auto big_ui = parser->get("-b");
    EXPECT_EQ(u_i.to<uint64_t>(), expect_ui);
    EXPECT_EQ(big_ui.to<uint64_t>(), expect_u_big_i);
    EXPECT_STREQ(u_i.inner().c_str(), std::to_string(expect_ui).c_str());
    EXPECT_STREQ(big_ui.inner().c_str(),
                 std::to_string(expect_u_big_i).c_str());
}

TEST(ArgparserFlagStore, ParseBool)
{
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag("--tr", "", ""));
    EXPECT_TRUE(parser->flag("--one", "", ""));
    EXPECT_TRUE(parser->flag("--fa", "", ""));
    EXPECT_TRUE(parser->flag("--zero", "", ""));
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
    auto t = parser->get("--tr");
    auto f = parser->get("--fa");
    auto one = parser->get("--one");
    auto zero = parser->get("--zero");
    EXPECT_STREQ(t.inner().c_str(), "true");
    EXPECT_STREQ(f.inner().c_str(), "false");
    EXPECT_STREQ(one.inner().c_str(), "1");
    EXPECT_STREQ(zero.inner().c_str(), "0");
    EXPECT_EQ(t.to<bool>(), true);
    EXPECT_EQ(f.to<bool>(), false);
    EXPECT_EQ(one.to<bool>(), true);
    EXPECT_EQ(zero.to<bool>(), false);
}
TEST(ArgparserFlagStore, ParseDouble)
{
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag("--d1", "", ""));
    EXPECT_TRUE(parser->flag("--d2", "", ""));
    EXPECT_TRUE(parser->flag("--d3", "", ""));
    EXPECT_TRUE(parser->flag("--d4", "", ""));
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
    auto d1 = parser->get("--d1");
    auto d2 = parser->get("--d2");
    auto d3 = parser->get("--d3");
    auto d4 = parser->get("--d4");
    EXPECT_STREQ(d1.inner().c_str(), "314.564");
    EXPECT_STREQ(d2.inner().c_str(), "-423.523");
    EXPECT_STREQ(d3.inner().c_str(), "1e8");
    EXPECT_STREQ(d4.inner().c_str(), "-1e6");
    EXPECT_DOUBLE_EQ(d1.to<double>(), 314.564);
    EXPECT_DOUBLE_EQ(d2.to<double>(), -423.523);
    EXPECT_DOUBLE_EQ(d3.to<double>(), 1e8);
    EXPECT_DOUBLE_EQ(d4.to<double>(), -1e6);
}

TEST(ArgparserFlagStore, ParseArray)
{
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag("--array", "-a", ""));
    const char *arg[] = {"./argtest", "--array", "1,2,3,4,5,6,7,8,9,10"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));

    auto arr = parser->get("--array");
    EXPECT_STREQ(arr.inner().c_str(), "1,2,3,4,5,6,7,8,9,10");

    auto arrs = arr.to<std::vector<int64_t>>();

    EXPECT_EQ(arrs.size(), 10);
    for (int i = 0; i < 10; ++i)
    {
        EXPECT_EQ(arrs[i], i + 1);
    }
}

TEST(ArgparserFlagStore, ParseStringArray)
{
    auto parser = argparser::new_parser();
    std::vector<std::string> expected = {
        "hello", "world", "my", "name", "is", "LiHua."};
    EXPECT_TRUE(parser->flag("--array", "-a", ""));
    const char *arg[] = {
        "./argtest", "--array", "hello,world,my,name,is,LiHua."};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));

    auto arr = parser->get("--array");
    EXPECT_STREQ(arr.inner().c_str(), "hello,world,my,name,is,LiHua.");

    auto arrs = arr.to<std::vector<std::string>>();

    ASSERT_EQ(arrs.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i)
    {
        EXPECT_EQ(arrs[i], expected[i]);
    }
}

TEST(ArgparserFlagStore, ParseStringWithComma)
{
    std::string expected = "hello,world,my,name,is,LiHua.";
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag("--str", "-s", ""));
    const char *arg[] = {"./argtest", "--str", expected.c_str()};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));

    auto str = parser->get("--str");
    EXPECT_STREQ(str.inner().c_str(), expected.c_str());

    auto same_str = str.to<std::string>();

    EXPECT_STREQ(same_str.c_str(), expected.c_str());
}

TEST(ArgparserFlagStore, ParseAnyArray)
{
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag("--array-i", "-i", ""));
    EXPECT_TRUE(parser->flag("--array-u", "-u", ""));
    EXPECT_TRUE(parser->flag("--array-d", "-d", ""));
    EXPECT_TRUE(parser->flag("--array-b", "-b", ""));
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

    auto i = parser->get("-i");
    EXPECT_STREQ(i.inner().c_str(), "1,-2,3,-4,5,-6,7,-8,9,-10");
    auto u = parser->get("-u");
    EXPECT_STREQ(u.inner().c_str(), "1,2,3,4,5");
    auto d = parser->get("-d");
    EXPECT_STREQ(d.inner().c_str(), "1.2,-3.4,5.6,-7.8");
    auto b = parser->get("-b");
    EXPECT_STREQ(b.inner().c_str(), "true,false,1,0");

    auto iarrs = i.to<std::vector<int>>();
    auto uarrs = u.to<std::vector<unsigned int>>();
    auto darrs = d.to<std::vector<double>>();
    auto barrs = b.to<std::vector<bool>>();

    EXPECT_TRUE(i.convertable_to<std::vector<unsigned int>>());
    EXPECT_TRUE(i.convertable_to<std::vector<double>>());
    EXPECT_FALSE(i.convertable_to<std::vector<bool>>());

    EXPECT_TRUE(u.convertable_to<std::vector<int>>());
    EXPECT_TRUE(u.convertable_to<std::vector<double>>());
    EXPECT_FALSE(u.convertable_to<std::vector<bool>>());

    EXPECT_FALSE(d.convertable_to<std::vector<int>>());
    EXPECT_FALSE(d.convertable_to<std::vector<unsigned int>>());
    EXPECT_FALSE(d.convertable_to<std::vector<bool>>());

    EXPECT_FALSE(b.convertable_to<std::vector<int>>());
    EXPECT_FALSE(b.convertable_to<std::vector<unsigned int>>());
    EXPECT_FALSE(b.convertable_to<std::vector<double>>());

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

TEST(ArgparserFlagStore, ParseArrayFailedAtMiddle)
{
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(
        "--array", "-a", "An array that has wrong middle elements"));
    const char *arg[] = {"./argtest", "-a", "1,2,t,4"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_FALSE(parser->get("--array").convertable_to<std::vector<int>>());
    EXPECT_FALSE(parser->get("-a").convertable_to<std::vector<int>>());
}
TEST(ArgparserFlagStore, ParseArrayFailedAtMiddle2)
{
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(
        "--array", "-a", "An array that has wrong middle elements"));
    // 2 is also not valid
    const char *arg[] = {"./argtest", "-a", "true,false,2,0"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
    EXPECT_FALSE(parser->get("--array").convertable_to<std::vector<bool>>());
    EXPECT_FALSE(parser->get("-a").convertable_to<std::vector<bool>>());
}
TEST(ArgparserFlagStore, ParseHas)
{
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(
        "--array", "-a", "An array that has wrong middle elements"));
    // 2 is also not valid
    const char *arg[] = {"./argtest", "-a", "true,false,2,0"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));

    EXPECT_TRUE(parser->has("--array"));
    EXPECT_TRUE(parser->has("-a"));
    EXPECT_FALSE(parser->has("--a"));
    EXPECT_FALSE(parser->has("-array"));
    EXPECT_FALSE(parser->has(""));
    EXPECT_FALSE(parser->has(" "));
}
TEST(ArgparserFlagStore, AllocatedFlagCanNotConflict)
{
    auto parser = argparser::new_parser();
    std::vector<bool> barrs;
    EXPECT_TRUE(parser->flag(
        &barrs, "--array", "-a", "An array that has wrong middle elements"));
    EXPECT_FALSE(parser->flag(
        "--array", "-a", "An array that has wrong middle elements"));
    const char *arg[] = {"./argtest", "-a", "true,false,1,0"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));

    // TODO: should use parser->store() to get the store.

    EXPECT_FALSE(parser->has("--array"));
    EXPECT_FALSE(parser->has("-a"));

    EXPECT_EQ(barrs.size(), 4);
    EXPECT_EQ(barrs[0], true);
    EXPECT_EQ(barrs[1], false);
    EXPECT_EQ(barrs[2], true);
    EXPECT_EQ(barrs[3], false);
}

TEST(ArgparserFlagStore, AllocatedFlagCanNotConflict2)
{
    auto parser = argparser::new_parser();
    std::vector<bool> tmp;
    EXPECT_TRUE(parser->flag(
        "--array", "-a", "An array that has wrong middle elements"));
    EXPECT_FALSE(parser->flag(
        &tmp, "--array", "-a", "An array that has wrong middle elements"));
    const char *arg[] = {"./argtest", "-a", "true,false,1,0"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));

    // TODO: should use parser->store() to get the store.
    EXPECT_TRUE(parser->has("--array"));
    EXPECT_TRUE(parser->has("-a"));

    auto barrs = parser->get("--array").to<std::vector<bool>>();

    EXPECT_EQ(barrs.size(), 4);
    EXPECT_EQ(barrs[0], true);
    EXPECT_EQ(barrs[1], false);
    EXPECT_EQ(barrs[2], true);
    EXPECT_EQ(barrs[3], false);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}