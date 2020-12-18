#ifndef CONVERT_H
#define CONVERT_H
#include <iostream>
namespace argparse
{
namespace convert
{
template <typename T>
bool apply_to(T *target, const std::string& value);

template <typename T>
T to(const std::string& input)
{
    T tmp;
    if (!apply_to<T>(&tmp, input))
    {
        std::cerr << "Failed to convert \"" << input << "\" to type "
                    << typeid(T).name() << std::endl;
        std::terminate();
    }
    return tmp;
}

template <typename T>
std::istream &operator>>(std::istream &is, std::vector<T> &vec)
{
    std::string token;
    vec.clear();
    while (!is.eof())
    {
        std::getline(is, token, ',');
        T tmp;
        bool succ = apply_to<T>(&tmp, token);
        if (!succ)
        {
            // manually set fail bit
            is.setstate(std::ios_base::failbit);
            return is;
        }
        vec.emplace_back(std::move(tmp));
    }
    return is;
}

template <typename T>
bool apply_to(T *target, const std::string &value)
{
    // A space is never allowed except that T is std::string.
    if (value.find(' ') != std::string::npos)
    {
        return false;
    }
    std::istringstream iss(value);
    iss >> *target;
    return iss.eof() && !iss.fail();
}

template <>
bool apply_to<bool>(bool *target, const std::string &value)
{
    if (value.find(' ') != std::string::npos)
    {
        return false;
    }

    if (value.empty() || value == "1" || value == "true")
    {
        *target = true;
        return true;
    }
    else if (value == "0" || value == "false")
    {
        *target = false;
        return true;
    }
    return false;
}

template <>
bool apply_to<std::string>(std::string *target, const std::string &value)
{
    *target = value;
    return true;
}

}  // namespace convert
}  // namespace argparse
#endif