#include <iostream>

#include "argparser/all.hpp"

int main(int argc, const char *argv[])
{
    uint64_t limit;
    int64_t time;
    uint64_t tries;
    bool init;
    auto &parser = argparser::init("An Example Command Line");
    parser.flag(&init, "--init", "-i", argparser::long_sentence, "false");
    parser.flag(&time, "--time", "-t", argparser::short_sentence, "0");
    parser.flag(&tries, "--tries", "", argparser::very_short_sentence, "0");
    parser.global_flag(&limit, "--limit", "-L", argparser::very_short_sentence, "0");

    uint64_t thread_nr;
    auto &sub_parser = parser.command(
        "run", "An Example Command Line that you expect it can run something");
    sub_parser.flag(
        &thread_nr, "--thread", "-t", argparser::short_sentence, "0");

    bool succ = parser.parse(argc, argv);
    if (succ)
    {
        parser.print_promt(argc, argv);
    }

    std::cout << "init: " << init << std::endl;
    std::cout << "time: " << time << std::endl;
    std::cout << "tries: " << tries << std::endl;
    std::cout << "thread_nr: " << thread_nr << std::endl;
    std::cout << "limit: " << limit << std::endl;
    using argparser::flag::operator<<;
    std::cout << "command path: " << parser.command_path() << std::endl;

    return 0;
}