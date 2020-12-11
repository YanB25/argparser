#include <iostream>
#include <vector>

#include "argparser/all.hpp"
int main(int argc, const char *argv[])
{
    auto &parser = argparser::init("A Hello-world Command-line Tool");

    std::vector<int> array;
    int size;

    parser.flag(&array, "--array", "-A", "An array of int as input", "1,2,3");
    parser.flag(&size, "--size", "-S", "The size of items", "0");

    int time;
    auto &run_parser =
        parser.command("run", "A sub-command, used to run something");
    run_parser.flag(&time, "--time", "-T", "The time to run", "5");

    parser.parse(argc, argv);
    parser.print_promt(argc, argv);

    return 0;
}