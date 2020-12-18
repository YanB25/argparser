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
    Flag(const std::string &full_name,
         const std::string &short_name,
         const std::string &desc)
        : full_name_(full_name), short_name_(short_name), desc_(desc)
    {
    }
    virtual ~Flag() = default;

    virtual bool apply(const std::string &value) = 0;

    virtual std::string short_name() const
    {
        return short_name_;
    }
    virtual std::string full_name() const
    {
        return full_name_;
    }
    virtual std::string desc() const
    {
        return desc_;
    }
    virtual bool match(const std::string & key) const
    {
        // never match an empty string.
        if (key.empty())
        {
            return false;
        }
        return key == full_name_ || key == short_name_;
    }
    virtual std::string to_string() const
    {
        return "{Flag " + full_name_ + ", " + short_name_ + "}";
    }

private:
    std::string full_name_;
    std::string short_name_;
    std::string desc_;
};
template <typename T>
class ConcreteFlag : public Flag
{
public:
    ConcreteFlag(T *flag,
                 const std::string &full_name,
                 const std::string &short_name,
                 const std::string &desc)
        : Flag(full_name, short_name, desc), flag_(flag)
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
    bool apply(const std::string &value) override
    {
        return apply_to(flag_, value);
    }
    static bool apply_to(T *target, const std::string &value);
    ~ConcreteFlag() = default;

protected:
    T *flag_;
};

class AllocatedFlag : public Flag
{
public:
    AllocatedFlag(const std::string &full_name,
                  const std::string &short_name,
                  const std::string &desc)
        : Flag(full_name, short_name, desc)
    {
    }
    static std::shared_ptr<AllocatedFlag> make_flag(
        const std::string &full_name,
        const std::string &short_name,
        const std::string &desc)
    {
        return std::make_shared<AllocatedFlag>(full_name, short_name, desc);
    }

    const std::string &inner() const
    {
        return inner_;
    }
    bool apply(const std::string &value) override
    {
        inner_ = value;
        return true;
    }

private:
    std::string inner_;
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