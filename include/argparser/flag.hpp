#ifndef ARG_PARSER_FLAG_H
#define ARG_PARSER_FLAG_H

#include <iostream>
#include <istream>
#include <string>

#include "./convert.hpp"

namespace argparser
{
namespace flag
{
class FlagStore;
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
    virtual bool match(const std::string &key) const
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
        std::optional<T> maybe = argparse::convert::try_to<T>(value);
        if (maybe.has_value())
        {
            *flag_ = std::move(maybe.value());
            return true;
        }
        return false;
    }
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

    template <typename T>
    T to() const
    {
        std::optional<T> maybe = argparse::convert::try_to<T>(inner_);
        if (maybe.has_value())
        {
            return maybe.value();
        }
        std::cerr << __FILE__ << ":" << __LINE__ << " Failed to convert \""
                  << inner_ << "\" to type " << typeid(T).name() << std::endl;
        std::terminate();
    }
    template <typename T>
    bool convertable_to() const
    {
        std::optional<T> maybe = argparse::convert::try_to<T>(inner_);
        return maybe.has_value();
    }
    const std::string &inner() const
    {
        return inner_;
    }

private:
    friend FlagStore;
    bool apply(const std::string &value) override
    {
        inner_ = value;
        return true;
    }

    std::string inner_;
};

template <>
std::string AllocatedFlag::to<std::string>() const
{
    return inner_;
}

template <>
bool AllocatedFlag::convertable_to<std::string>() const
{
    return !inner_.empty();
}

}  // namespace flag
}  // namespace argparser

#endif