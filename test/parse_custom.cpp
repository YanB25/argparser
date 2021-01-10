#include <inttypes.h>

#include "argparser/argparser.hpp"
#include "gtest/gtest.h"

class Bar
{
public:
    Bar(int64_t i64) : i64_(i64)
    {
    }
    Bar() = default;
    Bar(const Bar &rhs)
    {
        i64_ = rhs.i64_;
        has_copied_ = true;
        has_moved_ = false;
    }
    Bar(Bar &&rhs)
    {
        i64_ = std::move(rhs.i64_);
        has_moved_ = true;
        has_copied_ = false;
    }
    Bar &operator=(const Bar &rhs)
    {
        i64_ = rhs.i64_;
        has_copied_ = true;
        has_moved_ = false;
        return *this;
    }
    Bar &operator=(Bar &&rhs)
    {
        i64_ = std::move(rhs.i64_);
        has_moved_ = true;
        has_copied_ = false;
        return *this;
    }
    bool has_moved() const
    {
        return has_moved_;
    }
    bool has_copied() const
    {
        return has_copied_;
    }
    int64_t i64() const { return i64_; }
private:
    int64_t i64_;
    bool has_moved_ = false;
    bool has_copied_ = false;
};

template <>
std::optional<Bar> argparse::convert::try_to<Bar>(const std::string &input)
{
    if (input == "1")
    {
        return Bar(1);
    }
    else if (input == "2")
    {
        return Bar(2);
    }
    return {};
}

TEST(ArgparserFlag, CustomClassCanParsed)
{
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag("--bar", "-b", "The custom bar"));
    const char *arg[] = {"./argtest", "--bar", "1"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));

    auto &store = parser->store();
    ASSERT_TRUE(store.get("--bar").convertable_to<Bar>());
    auto bar = store.get("--bar").to<Bar>();
    EXPECT_TRUE(bar.has_copied());

    EXPECT_EQ(1, bar.i64());
}

TEST(ArgparserFlag, CustomClassCanParsedRegistered)
{
    Bar bar;
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&bar, "--bar", "-b", "The custom bar"));
    const char *arg[] = {"./argtest", "--bar", "1"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));

    EXPECT_EQ(1, bar.i64());
    EXPECT_TRUE(bar.has_moved());
}

TEST(ArgparserFlag, CustomClassCanParsed2)
{
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag("--bar", "-b", "The custom bar"));
    const char *arg[] = {"./argtest", "--bar", "2"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));

    auto &store = parser->store();
    ASSERT_TRUE(store.get("--bar").convertable_to<Bar>());
    auto bar = store.get("--bar").to<Bar>();

    EXPECT_EQ(2, bar.i64());
    EXPECT_TRUE(bar.has_copied());
}

TEST(ArgparserFlag, CustomClassCanParsedRegistered2)
{
    Bar bar;
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&bar, "--bar", "-b", "The custom bar"));
    const char *arg[] = {"./argtest", "--bar", "2"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));

    EXPECT_EQ(2, bar.i64());
    EXPECT_TRUE(bar.has_moved());
}

TEST(ArgparserFlag, CustomClassShouldFailToParsedRegistered)
{
    Bar bar;
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&bar, "--bar", "-b", "The custom bar"));
    const char *arg[] = {"./argtest", "--bar", "3"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}

TEST(ArgparserFlag, Unconvertable)
{
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag("--bar", "-b", "The custom bar"));
    const char *arg[] = {"./argtest", "--bar", "3"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));

    auto &store = parser->store();
    EXPECT_FALSE(store.get("--bar").convertable_to<Bar>());
}

TEST(ArgparserFlag, CustomClassEarlyAbortForUnconvertableDefault)
{
    Bar bar;
    auto parser = argparser::new_parser();
    EXPECT_FALSE(parser->flag(&bar, "--bar", "-b", "The custom bar", "3"));
    const char *arg[] = {"./argtest"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}

TEST(ArgparserFlag, CustomClassEarlyAbortForUnconvertableDefault2)
{
    Bar bar;
    auto parser = argparser::new_parser();
    EXPECT_FALSE(parser->flag(&bar, "--bar", "-b", "The custom bar", "3"));
    const char *arg[] = {"./argtest", "--bar", "1"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}

enum class Color
{
    kRed,
    kYellow,
    kBlue,
};

template<>
std::optional<Color> argparse::convert::try_to(const std::string& input)
{
    if (input == "red")
    {
        return Color::kRed;
    }
    else if (input == "yellow")
    {
        return Color::kYellow;
    }
    else if (input == "blue")
    {
        return Color::kBlue;
    }
    return {};
}
TEST(ArgparserFlag, EnumCanParsed)
{
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag("--color", "-c", "The color"));
    const char *arg[] = {"./argtest", "-c", "yellow"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));

    auto &store = parser->store();
    ASSERT_TRUE(store.get("--color").convertable_to<Color>());
    EXPECT_FALSE(store.get("--color").convertable_to<Bar>());
    EXPECT_FALSE(store.get("--color").convertable_to<int64_t>());
    auto c = store.get("-c").to<Color>();
    EXPECT_EQ(c, Color::kYellow);
}
TEST(ArgparserFlag, EnumCanParsedRegistered)
{
    Color c;
    auto parser = argparser::new_parser();
    EXPECT_TRUE(parser->flag(&c, "--color", "-c", "The color"));
    const char *arg[] = {"./argtest", "-c", "red"};
    EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));

    EXPECT_EQ(c, Color::kRed);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}