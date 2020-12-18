#include <iostream>
#include <vector>

#include "argparser/argparser.hpp"

class Bar
{
public:
    Bar(int data) : data_(data)
    {
    }
    void hello()
    {
        std::cout << "Hello! It is " << data_ << std::endl;
    }

private:
    int data_ = 0;
};

template <>
Bar argparse::convert::to<Bar>(const std::string &input)
{
    if (input == "Y")
    {
        return Bar(1);
    }
    else
    {
        return Bar(2);
    }
}

int main(int argc, const char *argv[])
{
    auto &parser = argparser::init("An example of custom convertor");

    parser.flag("--bar", "-b", "A custom class Bar");

    bool succ = parser.parse(argc, argv);
    if (!succ)
    {
        parser.print_promt(argc, argv);
        return 0;
    }
    
    auto& store = parser.store();
    auto bar = store.get("--bar").to<Bar>();
    bar.hello();

    return 0;
}