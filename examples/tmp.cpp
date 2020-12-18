#include "argparser/argparser.hpp"
#include <iostream>
#define EXPECT_TRUE(x) std::cout << #x << ", " << x << std::endl;
#define EXPECT_EQ(a, b) std::cout << #a ", " #b << ", " << a << ", "  << b << std::endl;
int main()
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