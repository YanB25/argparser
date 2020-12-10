#ifndef ARG_PARSER_COMMON_H
#define ARG_PARSER_COMMON_H
#include <cctype>
#include <string>
namespace argparser
{
namespace flag
{
inline static bool is_full_flag(const std::string &name)
{
    return name.size() >= 3 && name[0] == '-' && name[1] == '-' &&
           isalpha(name[2]);
}
inline static bool is_short_flag(const std::string &name)
{
    return name.empty() ||
           (name.size() >= 2 && name[0] == '-' && isalpha(name[1]));
}
inline static bool is_flag(const std::string &str)
{
    return is_full_flag(str) || is_short_flag(str);
}

}  // namespace flag
}  // namespace argparser
#endif