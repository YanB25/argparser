#include <iostream>

#include "argparser/all.hpp"

std::string long_paragraph =
    "Lorem ipsum dolor sit amet, consectetuer adipiscing elit."
    "Non eram nescius, Brute, cum, quae summis ingeniis exquisitaque "
    "doctrina philosophi Graeco sermone tractavissent, ea Latinis litteris "
    "mandaremus, fore ut hic noster labor in varias reprehensiones"
    "incurreret. Nam quibusdam, et iis quidem non admodum indoctis,"
    "totum hoc displicet philosophari.Quidam autem non tam id reprehendunt,"
    "si remissius agatur, sed tantum studium tamque multam operam ponendam in "
    "eo non arbitrantur.Erunt etiam, et ii quidem eruditi Graecis litteris, "
    "contemnentes Latinas, qui se dicant in Graecis legendis operam malle"
    "consumere.Postremo aliquos futuros suspicor,"
    "qui me ad alias litteras vocent, genus hoc scribendi,"
    "etsi sit elegans, personae tamen et dignitatis esse negent.";

int main(int argc, const char *argv[])
{
    std::string str;
    int64_t i64;
    uint64_t u64;
    bool b;
    std::vector<int64_t> arr_int64;
    std::vector<std::string> arr_str;
    auto parser = argparser::new_parser("");
    parser->flag(&str, "--string", "-s", "A String", "default");
    parser->flag(&i64, "--int64", "-i", "An int 64", "0");
    parser->flag(&u64, "--uint64", "-u", "An uint 64", "0");
    parser->flag(&b, "--bool", "-b", "A bool", "false");
    parser->flag(&arr_int64, "--array-int64", "", "An array of int64", "1,2,3,4,5");
    parser->flag(&arr_str, "--array-str", "", "An array of string", "a,b,c,d,e");
    
    parser->parse(argc, argv);
    parser->print_promt(argc, argv);

    std::cout << "string: " << str << std::endl;
    std::cout << "int64: " << i64 << std::endl;
    std::cout << "uint64: " << u64 << std::endl;
    std::cout << "bool: " << b << std::endl;

    using argparser::flag::operator<<;
    std::cout << "array-int64 " << arr_int64 << std::endl;
    std::cout << "array-str " << arr_str << std::endl;
}
// int main(int argc, const char *argv[])
// {
// #define EXPECT_TRUE(x) std::cout << "expect true: " << x << std::endl
// #define EXPECT_FALSE(x) std::cout << "expect false: " << x << std::endl
// #define EXPECT_EQ(x, y)                           \
//     {                                             \
//         std::cout << (x) << ", " << (y) << std::endl; \
//     }
//     std::vector<int64_t> iarrs;
//     std::vector<uint64_t> uarrs;
//     std::vector<double> darrs;
//     std::vector<bool> barrs;
//     auto parser = argparser::new_parser("parse bool");
//     EXPECT_TRUE(parser->flag(&barrs, "--array-b", "-b", ""));
//     const char *arg[] = {"./argtest",
//                          "-b",
//                          "true,false,1,0"};
//     EXPECT_TRUE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
//     EXPECT_EQ(barrs.size(), 4);
//     EXPECT_EQ(barrs[0], true);
//     EXPECT_EQ(barrs[1], false);
//     EXPECT_EQ(barrs[2], true);
//     EXPECT_EQ(barrs[3], false);
// }