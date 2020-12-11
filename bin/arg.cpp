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

// int main(int argc, const char *argv[])
// {
//     auto &parser = argparser::init("Simple Commandline tool");
//     uint64_t time;
//     std::string name;
//     bool has_init = true;
//     uint64_t tired;
//     int64_t t2;
//     double d;
//     std::vector<int64_t> vec;
//     parser.flag(&time, "--time", "-t", long_paragraph, "4");
//     parser.flag(&d, "--d", "", "", "5.2");
//     parser.flag(&name, "--name", "-n", long_paragraph.substr(300), "default");
//     parser.flag(&has_init, "--init", "-i", long_paragraph.substr(500), "true");
//     parser.flag(&tired, "--tired", "", long_paragraph.substr(700), "4");
//     parser.flag(&t2, "--t2", "", long_paragraph.substr(700), "-1");
//     parser.flag(&vec, "--vec", "", "");
//     auto &start_parser =
//         parser.command("start", "start a node in a multi-node cluster");
//     start_parser.flag(&t2, "--test", "-t", long_paragraph.substr(500));
//     start_parser.command("tpcc", "start tpcc workload");
//     parser.parse(argc, argv);
//     parser.print_promt(argc, argv);
//     std::cout << "time is " << time << std::endl;
//     std::cout << "name is " << name << std::endl;
//     std::cout << "init is " << has_init << std::endl;
//     std::cout << "t2 is " << t2 << std::endl;
//     std::cout << "has_init is " << has_init << std::endl;
//     std::cout << "d is " << d << std::endl;
//     std::cout << "vec size is " << vec.size() << std::endl;
// }
int main(int argc, const char *argv[])
{
#define EXPECT_TRUE(x) std::cout << "expect true: " << x << std::endl
#define EXPECT_FALSE(x) std::cout << "expect false: " << x << std::endl
#define EXPECT_EQ(x, y)                           \
    {                                             \
        std::cout << x << ", " << y << std::endl; \
    }
    int64_t flag = 1;
    auto parser = argparser::new_parser("");
    // failed to register, default value "" not parsable
    EXPECT_FALSE(parser->flag(&flag, "--f", "", "", ""));
    const char *arg[] = {"./argtest", "--f", "5"};
    EXPECT_FALSE(parser->parse(sizeof(arg) / sizeof(arg[0]), arg));
}