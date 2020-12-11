#include <iostream>

#include "argparser/all.hpp"

int main(int argc, const char *argv[])
{
    uint64_t limit;
    std::string str;
    int64_t i64;
    uint64_t u64;
    bool b;
    double d;
    std::vector<int64_t> arr_int64;
    std::vector<std::string> arr_str;
    auto parser = argparser::new_parser("");
    parser->global_flag(&limit, "--limit", "-L", "The limit of the server", "-1");
    parser->flag(&str, "--string", "-s", "A string", "default");
    parser->flag(&i64, "--int64", "-i", "An int64", "0");
    parser->flag(&u64, "--uint64", "-u", "An uint64", "0");
    parser->flag(&b, "--bool", "-b", "A bool", "false");
    parser->flag(&d, "--double", "-d", "A double", "-5.8");
    parser->flag(&arr_int64, "--array-int64", "", "An array of int64", "1,2,3,4,5");
    parser->flag(&arr_str, "--array-str", "", "An array of string", "a,b,c,d,e");
    
    parser->parse(argc, argv);
    parser->print_promt(argc, argv);

    std::cout << "limit: " << limit << std::endl;
    std::cout << "string: " << str << std::endl;
    std::cout << "int64: " << i64 << std::endl;
    std::cout << "uint64: " << u64 << std::endl;
    std::cout << "bool: " << b << std::endl;
    std::cout << "double: " << d << std::endl;

    using argparser::flag::operator<<;
    std::cout << "array-int64 " << arr_int64 << std::endl;
    std::cout << "array-str " << arr_str << std::endl;
}