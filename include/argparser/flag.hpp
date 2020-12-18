#ifndef ARG_PARSER_FLAG_H
#define ARG_PARSER_FLAG_H

#include <iostream>
#include <istream>
#include <string>

namespace argparser
{
namespace flag
{
class Flag
{
public:
    using Pointer = std::shared_ptr<Flag>;
    Flag() = default;
    virtual ~Flag() = default;
    virtual bool match(const std::string &) const = 0;
    virtual std::string to_string() const = 0;
    virtual bool apply(const std::string &value) = 0;
    virtual std::string short_name() const = 0;
    virtual std::string full_name() const = 0;
    virtual std::string desc() const = 0;
};
template <typename T>
class ConcreteFlag : public Flag
{
public:
    ConcreteFlag(T *flag,
                 const std::string &full_name,
                 const std::string &short_name,
                 const std::string &desc)
        : flag_(flag),
          full_name_(full_name),
          short_name_(short_name),
          desc_(desc)
    {
    }
    static std::shared_ptr<ConcreteFlag<T>> make_flag(
        T *flag,
        const std::string &full_name,
        const std::string &short_name,
        const std::string &desc)
    {
        return std::make_shared<ConcreteFlag<T>>(
            flag, full_name, short_name, desc);
    }
    bool match(const std::string &key) const override
    {
        // never match an empty string.
        if (key.empty())
        {
            return false;
        }
        return key == full_name_ || key == short_name_;
    }
    std::string to_string() const override
    {
        return "{Flag " + full_name_ + ", " + short_name_ + "}";
    }
    bool apply(const std::string &value) override
    {
        return apply_to(flag_, value);
    }
    static bool apply_to(T *target, const std::string &value);
    std::string short_name() const override
    {
        return short_name_;
    }
    std::string full_name() const override
    {
        return full_name_;
    }
    std::string desc() const override
    {
        return desc_;
    }
    ~ConcreteFlag() = default;

private:
    T *flag_;
    std::string full_name_;
    std::string short_name_;
    std::string desc_;
};

class AllocatedFlag : public ConcreteFlag<std::string>
{
public:
    AllocatedFlag(const std::string &full_name,
                  const std::string &short_name,
                  const std::string &desc)
        : ConcreteFlag(&inner, full_name, short_name, desc)
    {
    }
    static std::shared_ptr<AllocatedFlag> make_flag(
        const std::string &full_name,
        const std::string &short_name,
        const std::string &desc)
    {
        return std::make_shared<AllocatedFlag>(full_name, short_name, desc);
    }

private:
    std::string inner;
};

template <typename T>
std::istream &operator>>(std::istream &is, std::vector<T> &vec)
{
    std::string token;
    vec.clear();
    while (!is.eof())
    {
        std::getline(is, token, ',');
        T tmp;
        bool succ = ConcreteFlag<T>::apply_to(&tmp, token);
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
bool ConcreteFlag<T>::apply_to(T *target, const std::string &value)
{
    if (value.find(' ') != std::string::npos)
    {
        return false;
    }
    std::istringstream iss(value);
    iss >> *target;
    return iss.eof() && !iss.fail();
}

template <>
bool ConcreteFlag<bool>::apply_to(bool *target, const std::string &value)
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
bool ConcreteFlag<std::string>::apply_to(std::string *target,
                                         const std::string &value)
{
    *target = value;
    return true;
}

}  // namespace flag
}  // namespace argparser

#endif