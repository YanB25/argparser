#ifndef ARG_PARSER_ALL_H
#define ARG_PARSER_ALL_H

#include <string>

#ifndef ARG_PARSER_H
#define ARG_PARSER_H
#include <cctype>
#include <iostream>
#include <list>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <unordered_map>
#include <vector>

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
    return name.size() >= 2 && name[0] == '-' && isalpha(name[1]);
}
inline static bool is_flag(const std::string &str)
{
    return is_full_flag(str) || is_short_flag(str);
}
std::vector<std::string> split(std::string str,
                               const std::string& delimiter)
{
    std::vector<std::string> ret;
    size_t pos = 0;
    std::string token;
    while ((pos = str.find(delimiter)) != std::string::npos)
    {
        token = str.substr(0, pos);
        ret.emplace_back(std::move(token));
        str.erase(0, pos + delimiter.length());
    }
    ret.push_back(std::move(str));
    return ret;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
    os << "[";
    for (size_t i = 0; i < vec.size(); ++i)
    {
        os << vec[i];
        if (i != vec.size() - 1)
        {
            os << ", ";
        }
    }
    os << "]";
    return os;
}
std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& vec)
{
    os << "[";
    for (size_t i = 0; i < vec.size(); ++i)
    {
        os << "\"" << vec[i] << "\"";
        if (i != vec.size() - 1)
        {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

}  // namespace flag
}  // namespace argparser
#endif
#ifndef DEBUG_H
#define DEBUG_H
#include <ostream>
#include <list>
#include <string>
std::ostream& operator<<(std::ostream& os, std::list<std::pair<std::string, std::string>> pairs)
{
    if (!pairs.empty())
    {
        os << "[";
        for (const auto&[first, second]: pairs)
        {
            os << "{" << first << ", " << second << "}, ";
        }
        os << "]" << std::endl;
    }
    else
    {
        os << "[]" << std::endl;
    }
    return os;
}
#endif
#ifndef ARG_PARSER_MANAGER_H
#define ARG_PARSER_MANAGER_H

#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#ifndef ARG_PARSER_FLAG_H
#define ARG_PARSER_FLAG_H

#include <iostream>
#include <istream>
#include <string>

#ifndef CONVERT_H
#define CONVERT_H
#include <iostream>
#include <optional>
namespace argparse
{
namespace convert
{
template <typename T>
bool apply_to(T *target, const std::string &value);

template <typename T>
std::optional<T> try_to(const std::string &input)
{
    T tmp;
    if (!apply_to<T>(&tmp, input))
    {
        return {};
    }
    return tmp;
}

template <typename T>
std::istream &operator>>(std::istream &is, std::vector<T> &vec)
{
    std::string token;
    vec.clear();
    if (is.peek() == EOF)
    {
        // parse empty string "" to an array
        // success and the result is an empty array
        return is;
    }
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

namespace argparser
{
namespace flag
{
class FlagStore
{
    using Required = bool;
    using Applied = bool;
    using Meta = std::pair<Required, Applied>;

public:
    using Pointer = std::shared_ptr<FlagStore>;
    static Pointer new_instance()
    {
        return std::make_unique<FlagStore>();
    }
    template <typename T>
    bool add_flag(T *slot,
                  const std::string &full_name,
                  const std::string &short_name,
                  const std::string &desc,
                  const std::optional<std::string> &default_val,
                  bool required)
    {
        auto pair = Meta(required, false);
        flags_.emplace_back(
            flag::ConcreteFlag<T>::make_flag(slot, full_name, short_name, desc),
            std::move(pair));
        if (default_val.has_value())
        {
            auto &flag = flags_.back().first;
            if (!flag->apply(default_val.value()))
            {
                std::cerr << "Failed to register flag " << full_name << ": "
                          << "default value \"" << default_val.value()
                          << "\" not parsable" << std::endl;
                // pop the error one
                flags_.pop_back();
                return false;
            }
        }

        max_full_name_len_ = std::max(max_full_name_len_, full_name.size());
        max_short_name_len_ = std::max(max_short_name_len_, short_name.size());
        return true;
    }
    bool add_flag(const std::string &full_name,
                  const std::string &short_name,
                  const std::string &desc,
                  const std::optional<std::string> &default_val,
                  bool required)
    {
        auto meta = Meta(required, false);
        allocated_flags_.emplace_back(
            flag::AllocatedFlag(full_name, short_name, desc), std::move(meta));
        if (default_val.has_value())
        {
            auto &allocated_flag = allocated_flags_.back().first;
            if (!allocated_flag.apply(default_val.value()))
            {
                std::cerr << "Failed to register flag " << full_name << ": "
                          << "default value \"" << default_val.value()
                          << "\" not parsable" << std::endl;
                // pop the error one
                allocated_flags_.pop_back();
                return false;
            }
        }

        max_full_name_len_ = std::max(max_full_name_len_, full_name.size());
        max_short_name_len_ = std::max(max_short_name_len_, short_name.size());
        return true;
    }
    bool empty() const
    {
        return flags_.empty() && allocated_flags_.empty();
    }
    bool apply(const std::string &key, const std::string &value)
    {
        for (auto &[flag, meta] : flags_)
        {
            bool &applied = meta.second;
            if (flag->match(key))
            {
                if (applied)
                {
                    std::cerr << "Failed to apply " << key << "=\"" << value
                              << "\": "
                              << "Flag " << key
                              << " already set and is provided more than once."
                              << std::endl;
                    return false;
                }
                if (!flag->apply(value))
                {
                    std::cerr << "Failed to apply " << key << "=\"" << value
                              << "\": \"" << value << "\" not parsable"
                              << std::endl;
                    return false;
                }
                applied = true;
                return true;
            }
        }
        for (auto &[flag, meta] : allocated_flags_)
        {
            bool &applied = meta.second;
            if (flag.match(key))
            {
                if (applied)
                {
                    std::cerr << "Failed to apply " << key << "=\"" << value
                              << "\": "
                              << "Flag " << key
                              << " already set and is provided more than once."
                              << std::endl;
                    return false;
                }
                if (!flag.apply(value))
                {
                    std::cerr << "Failed to apply " << key << "=\"" << value
                              << "\": \"" << value << "\" not parsable"
                              << std::endl;
                    return false;
                }
                applied = true;
                return true;
            }
        }
        return false;
    }
    bool contain(const std::string name) const
    {
        for (const auto &flagline : flags_)
        {
            if (flagline.first->match(name))
            {
                return true;
            }
        }
        for (const auto &flagline : allocated_flags_)
        {
            if (flagline.first.match(name))
            {
                return true;
            }
        }
        return false;
    }
    const flag::AllocatedFlag &get(const std::string &name) const
    {
        for (const auto &[flag, meta] : allocated_flags_)
        {
            std::ignore = meta;
            if (flag.match(name))
            {
                return flag;
            }
        }
        std::cerr << "Failed to get " << name << ": not found." << std::endl;
        std::terminate();
    }
    bool has(const std::string &name) const
    {
        for (const auto &[flag, meta] : allocated_flags_)
        {
            std::ignore = meta;
            if (flag.match(name))
            {
                return true;
            }
        }
        return false;
    }
    size_t size() const
    {
        return flags_.size() + allocated_flags_.size();
    }
    using FlagId = std::tuple<std::string, std::string>;
    std::vector<FlagId> missing_keys() const
    {
        std::vector<FlagId> ret;
        for (const auto &[flag, meta] : flags_)
        {
            const bool &required = meta.first;
            const bool &applied = meta.second;
            if (required && !applied)
            {
                ret.emplace_back(flag->full_name(), flag->short_name());
            }
        }
        for (const auto &[flag, meta] : allocated_flags_)
        {
            const bool &required = meta.first;
            const bool &applied = meta.second;
            if (required && !applied)
            {
                ret.emplace_back(flag.full_name(), flag.short_name());
            }
        }
        return ret;
    }
    // TODO: make it formator
    void print_flags(const std::string &title = "Flags") const
    {
        if (empty())
        {
            return;
        }
        std::cout << title << ":" << std::endl;
        for (const auto &[flag, meta] : flags_)
        {
            std::ignore = meta;
            auto short_name = flag->short_name();
            auto full_name = flag->full_name();
            std::cout << "  ";
            std::cout << std::string(max_short_name_len_ - short_name.size(),
                                     ' ');
            std::cout << short_name << (short_name.empty() ? "  " : ", ");
            std::cout << full_name;
            std::cout << std::endl;
            size_t padding_len =
                2 + max_short_name_len_ + 2 + max_full_name_len_ + 2;
            auto padding_str = std::string(padding_len, ' ');
            std::cout << padding_str;
            auto desc = flag->desc();
            size_t pos = 0;
            std::string token;
            size_t current_column = 0;
            while ((pos = desc.find(" ")) != std::string::npos)
            {
                token = desc.substr(0, pos);
                current_column += token.size();
                if (current_column >= 80)
                {
                    current_column = 0;
                    std::cout << std::endl << padding_str;
                }
                std::cout << token << " ";
                desc.erase(0, pos + 1);
            }
            std::cout << desc;
            std::cout << std::endl << std::endl;
        }
        for (const auto &[flag, meta] : allocated_flags_)
        {
            std::ignore = meta;
            auto short_name = flag.short_name();
            auto full_name = flag.full_name();
            std::cout << "  ";
            std::cout << std::string(max_short_name_len_ - short_name.size(),
                                     ' ');
            std::cout << short_name << (short_name.empty() ? "  " : ", ");
            std::cout << full_name;
            std::cout << std::endl;
            size_t padding_len =
                2 + max_short_name_len_ + 2 + max_full_name_len_ + 2;
            auto padding_str = std::string(padding_len, ' ');
            std::cout << padding_str;
            auto desc = flag.desc();
            size_t pos = 0;
            std::string token;
            size_t current_column = 0;
            while ((pos = desc.find(" ")) != std::string::npos)
            {
                token = desc.substr(0, pos);
                current_column += token.size();
                if (current_column >= 80)
                {
                    current_column = 0;
                    std::cout << std::endl << padding_str;
                }
                std::cout << token << " ";
                desc.erase(0, pos + 1);
            }
            std::cout << desc;
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    FlagStore() = default;

    ~FlagStore() = default;

private:
    // FlagLine = {pointer, {bool, bool}}
    using FlagLine = std::pair<flag::Flag::Pointer, Meta>;
    using AllocatedFlagLine = std::pair<flag::AllocatedFlag, Meta>;
    std::vector<FlagLine> flags_;
    std::vector<AllocatedFlagLine> allocated_flags_;

    std::vector<bool> required_;
    std::vector<bool> applied_;

    size_t max_full_name_len_{0};
    size_t max_short_name_len_{0};
};

}  // namespace flag
}  // namespace argparser
#endif
#ifndef FLAG_VALIDATOR_H
#define FLAG_VALIDATOR_H
#include <iostream>
#include <set>
#include <string>

namespace argparser
{
namespace flag
{
class Validator
{
public:
    Validator(flag::FlagStore::Pointer gf_store) : gf_store_(gf_store)
    {
    }
    bool validate(const std::string &full_name, const std::string &short_name)
    {
        if (!flag::is_full_flag(full_name) && !flag::is_short_flag(short_name))
        {
            std::cerr << "Failed to register flag " << full_name << ", "
                      << short_name << ": Both flag formats are not allowed"
                      << std::endl;
            return false;
        }
        if (!full_name.empty() && !flag::is_full_flag(full_name))
        {
            std::cerr << "Failed to register flag " << full_name
                      << ": identity not allowed" << std::endl;
            return false;
        }
        if (!short_name.empty() && !flag::is_short_flag(short_name))
        {
            std::cerr << "Failed to register flag " << short_name << "("
                      << full_name << ")"
                      << ": identity not allowed" << std::endl;
            return false;
        }
        if (!unique_full_flag(full_name))
        {
            std::cerr << "Failed to register flag " << full_name
                      << ": flag already registered" << std::endl;
            return false;
        }
        if (!unique_short_flag(short_name))
        {
            std::cerr << "Failed to register flag " << short_name << "("
                      << full_name << ")"
                      << ": flag already registered" << std::endl;
            return false;
        }
        if (gf_store_->contain(full_name) || gf_store_->contain(short_name))
        {
            std::cerr << "Flag registered failed: flag \"" << full_name
                      << "\", \"" << short_name
                      << "\" conflict with global flag" << std::endl;
            return false;
        }
        if (!full_name.empty())
        {
            inserted_full_names_.insert(full_name);
        }
        if (!short_name.empty())
        {
            inserted_short_names_.insert(short_name);
        }
        return true;
    }
    bool unique_full_flag(const std::string &name) const
    {
        return inserted_full_names_.find(name) == inserted_full_names_.end();
    }
    bool unique_short_flag(const std::string &name) const
    {
        return inserted_short_names_.find(name) == inserted_short_names_.end();
    }

private:
    std::set<std::string> inserted_full_names_;
    std::set<std::string> inserted_short_names_;
    flag::FlagStore::Pointer gf_store_;
};
}  // namespace flag
}  // namespace argparser
#endif
namespace argparser
{
class Parser;
class ParserStore
{
public:
    using Pointer = std::unique_ptr<ParserStore>;
    ParserStore() = default;
    ~ParserStore() = default;
    ParserStore(const ParserStore &) = delete;
    ParserStore(ParserStore &&) = delete;
    ParserStore &operator=(const ParserStore &) = delete;
    ParserStore &operator=(ParserStore &&) = delete;
    const flag::AllocatedFlag &get(const std::string &name) const
    {
        if (flag_store_->has(name))
        {
            return flag_store_->get(name);
        }
        return gf_store_->get(name);
    }
    bool has(const std::string &name) const
    {
        return flag_store_->has(name) || gf_store_->has(name);
    }

private:
    friend Parser;
    void link_flag_store(flag::FlagStore::Pointer p)
    {
        flag_store_ = p;
    }
    void link_global_flag_store(flag::FlagStore::Pointer p)
    {
        gf_store_ = p;
    }
    flag::FlagStore::Pointer flag_store_;
    flag::FlagStore::Pointer gf_store_;
};
class Parser
{
public:
    using Pointer = std::unique_ptr<Parser>;
    using FlagPair = std::pair<std::string, std::string>;
    using FlagPairs = std::list<FlagPair>;
    Parser(std::shared_ptr<flag::FlagStore> global_flag_store,
           const char *description)
        : description_(description),
          flag_store_(flag::FlagStore::new_instance()),
          gf_store_(global_flag_store),
          validator_(gf_store_),
          store_(std::make_unique<ParserStore>())
    {
        store_->link_global_flag_store(gf_store_);
        store_->link_flag_store(flag_store_);
    }
    Parser(const Parser &) = delete;
    Parser(Parser &&) = delete;
    Parser &operator=(const Parser &) = delete;
    Parser &operator=(Parser &&) = delete;

    void print_promt() const
    {
        if (!init_)
        {
            std::cerr << "ERR: parse() not called." << std::endl;
            return;
        }
        std::cout << description_ << std::endl << std::endl;

        print_usage();
        print_command();
        flag_store_->print_flags();
        gf_store_->print_flags("Global Flag");
    }
    void print_promt(int argc, const char *argv[]) const
    {
        auto pairs = retrieve(argc, argv);
        return print_promt(pairs);
    }
    std::string desc() const
    {
        return description_;
    }
    Parser &command(const std::string &command, const char *desc = "")
    {
        sub_parsers_.emplace(command,
                             std::make_unique<Parser>(gf_store_, desc));
        max_command_len_ = std::max(max_command_len_, command.size());
        return *sub_parsers_[command];
    }
    // TODO: make default has type?
    template <typename T>
    bool flag(T *flag,
              const char *full_name,
              const char *short_name,
              const char *desc,
              const char *default_val)
    {
        if (!validator_.validate(full_name, short_name))
        {
            return false;
        }
        return flag_store_->add_flag(
            flag, full_name, short_name, desc, default_val, false);
    }
    template <typename T>
    bool flag(T *flag,
              const char *full_name,
              const char *short_name,
              const char *desc)
    {
        if (!validator_.validate(full_name, short_name))
        {
            return false;
        }
        return flag_store_->add_flag(
            flag, full_name, short_name, desc, std::nullopt, true);
    }
    /**
     * This function register flag to the internal FlagStore.
     * The user can later retrieve the flag via
     * int result = FlagStore::instance().get("--flag").to<int>();
     */
    bool flag(const char *full_name, const char *short_name, const char *desc)
    {
        if (!validator_.validate(full_name, short_name))
        {
            return false;
        }
        return flag_store_->add_flag(
            full_name, short_name, desc, std::nullopt, true);
    }
    bool flag(const char *full_name,
              const char *short_name,
              const char *desc,
              const char *default_val)
    {
        if (!validator_.validate(full_name, short_name))
        {
            return false;
        }
        return flag_store_->add_flag(
            full_name, short_name, desc, default_val, false);
    }
    template <typename T>
    bool global_flag(T *flag,
                     const char *full_name,
                     const char *short_name,
                     const char *desc)
    {
        if (!validator_.validate(full_name, short_name))
        {
            return false;
        }
        return gf_store_->add_flag(
            flag, full_name, short_name, desc, std::nullopt, true);
    }
    template <typename T>
    bool global_flag(T *flag,
                     const char *full_name,
                     const char *short_name,
                     const char *desc,
                     const char *default_val)
    {
        if (!validator_.validate(full_name, short_name))
        {
            return false;
        }
        return gf_store_->add_flag(
            flag, full_name, short_name, desc, default_val, false);
    }
    /**
     * This function register flag to the internal FlagStore.
     * The user can later retrieve the flag via
     * int result = FlagStore::instance().get("--flag").to<int>();
     */
    bool global_flag(const char *full_name,
                     const char *short_name,
                     const char *desc)
    {
        if (!validator_.validate(full_name, short_name))
        {
            return false;
        }
        return gf_store_->add_flag(
            full_name, short_name, desc, std::nullopt, true);
    }
    bool global_flag(const char *full_name,
                     const char *short_name,
                     const char *desc,
                     const char *default_val)
    {
        if (!validator_.validate(full_name, short_name))
        {
            return false;
        }
        return gf_store_->add_flag(
            full_name, short_name, desc, default_val, false);
    }
    bool parse(int argc, const char *argv[])
    {
        command_path_.clear();
        program_name = argv[0];

        auto pairs = retrieve(argc, argv);
        return do_parse(pairs, store_, command_path_);
    }
    std::vector<std::string> command_path() const
    {
        return command_path_;
    }
    const ParserStore &store() const
    {
        return *store_;
    }

private:
    bool init_{false};
    std::string program_name;
    std::string description_;

    flag::FlagStore::Pointer flag_store_;
    flag::FlagStore::Pointer gf_store_;
    std::unordered_map<std::string, Pointer> sub_parsers_;
    size_t max_command_len_{0};

    flag::Validator validator_;

    std::vector<std::string> command_path_;
    ParserStore::Pointer store_;

    void print_usage() const
    {
        if (!flag_store_->empty() || !sub_parsers_.empty())
        {
            std::cout << "Usage:" << std::endl;
        }
        if (!sub_parsers_.empty())
        {
            std::cout << std::string(8, ' ') << program_name << " [command]"
                      << std::endl;
        }
        if (!flag_store_->empty())
            std::cout << std::string(8, ' ') << program_name << " [flag]"
                      << std::endl;
        if (!flag_store_->empty() || !sub_parsers_.empty())
        {
            std::cout << std::endl;
        }
    }
    void print_promt(FlagPairs &pairs) const
    {
        for (auto it = pairs.begin(); it != pairs.end(); ++it)
        {
            const auto &key = it->first;
            if (flag::is_flag(key))
            {
                if (flag_store_->contain(key))
                {
                    // this flag is expected, we can keep going.
                    continue;
                }
                else
                {
                    // this flag is not expected, stop here.
                    break;
                }
            }
            else
            {
                // this is a command
                auto parser_it = sub_parsers_.find(key);
                if (parser_it == sub_parsers_.end())
                {
                    break;
                }
                pairs.erase(pairs.begin(), ++it);
                return parser_it->second->print_promt(pairs);
            }
        }
        print_promt();
    }
    const flag::FlagStore::Pointer flag_store() const
    {
        return flag_store_;
    }
    void print_command() const
    {
        if (!sub_parsers_.empty())
        {
            std::cout << "Available Commands:" << std::endl;
            for (const auto &[command, parser] : sub_parsers_)
            {
                std::cout << "  " << command;
                size_t printed_length = 2 + command.length();
                std::cout << std::string(
                    max_command_len_ + 2 + 2 - printed_length, ' ');
                auto desc = parser->desc();

                size_t pos = 0;
                std::string token;
                size_t current_column = 0;
                while ((pos = desc.find(" ")) != std::string::npos)
                {
                    token = desc.substr(0, pos);
                    current_column += token.size();
                    if (current_column >= 60)
                    {
                        current_column = 0;
                        std::cout << std::endl
                                  << std::string(max_command_len_ + 2 + 2, ' ');
                    }
                    std::cout << token << " ";
                    desc.erase(0, pos + 1);
                }
                std::cout << desc << std::endl;
                // std::cout << parser->desc() << std::endl;
            }
            std::cout << std::endl;
        }
    }
    bool do_parse(FlagPairs &pairs,
                  ParserStore::Pointer &store,
                  std::vector<std::string> &command_path)
    {
        init_ = true;
        store->link_flag_store(flag_store_);

        for (auto pair_it = pairs.begin(); pair_it != pairs.end(); ++pair_it)
        {
            const auto &key = pair_it->first;
            const auto &value = pair_it->second;

            /**
             * It is either a flag or a command.
             * A flag: ./program --test
             * A command: ./program start
             *
             * If it is a command, we delegate the parsing to the sub_parser
             */
            if (!flag::is_flag(key))
            {
                auto parser_it = sub_parsers_.find(key);
                if (parser_it == sub_parsers_.end())
                {
                    std::cerr << "Failed to parse command \"" << key
                              << "\": use --help for usage." << std::endl;
                    return false;
                }
                auto sub_parser = parser_it->second.get();
                command_path.push_back(key);
                // remove [begin, pair_it] and pass the remaining to the
                // sub_parser
                pairs.erase(pairs.begin(), ++pair_it);
                return sub_parser->do_parse(pairs, store, command_path);
            }

            if (!flag_store_->apply(key, value) &&
                !gf_store_->apply(key, value))
            {
                std::cerr << "Failed to apply " << key << "=\"" << value
                          << "\": Failure due to previous problem" << std::endl;
                return false;
            }
        }

        auto missing_keys = flag_store_->missing_keys();
        if (!missing_keys.empty())
        {
            std::cerr << "Failed to parse command line: [";
            for (const auto &[full_name, short_name] : missing_keys)
            {
                std::cerr << "{Flag " << full_name << ", " << short_name
                          << "}, ";
            }
            std::cerr << "] are required but not provided." << std::endl;
            return false;
        }
        return true;
    }
    static FlagPairs retrieve(int argc, const char *argv[])
    {
        FlagPairs ret;
        // skip program name, i start from 1
        for (int i = 1; i < argc; ++i)
        {
            std::string command_opt(argv[i]);
            /**
             * It is a command.
             * e.g. ./program test
             */
            if (!flag::is_flag(command_opt))
            {
                ret.emplace_back(std::move(command_opt), "");
                continue;
            }

            /**
             * separate:
             * --time 5
             * not separate:
             * --time=5
             */
            std::string key;
            std::string value;
            auto equal_pos = command_opt.find('=');
            bool separate = equal_pos == std::string::npos;
            if (!separate)
            {
                key = command_opt.substr(0, equal_pos);
                value = command_opt.substr(equal_pos + 1);
            }
            else
            {
                /**
                 * value may be the next argv
                 * e.g. --time 5
                 *
                 * or it's a flag with no value
                 * e.g. --flag_on
                 */
                key = command_opt;
                if (i + 1 >= argc || flag::is_flag(argv[i + 1]))
                {
                    // no value.
                }
                else
                {
                    // value is the next argv
                    value = argv[i + 1];
                    // skip the next argv
                    i++;
                }
            }
            ret.emplace_back(key, value);
        }
        return ret;
    }
};  // namespace argparser
std::shared_ptr<Parser> new_parser(const char *desc = "")
{
    auto global_flag_store = std::make_shared<flag::FlagStore>();
    return std::make_shared<Parser>(global_flag_store, desc);
}

Parser &init(const char *desc)
{
    static std::shared_ptr<Parser> root_parser;
    if (root_parser == nullptr)
    {
        root_parser = new_parser(desc);
    }
    return *root_parser;
}
namespace impl
{
std::shared_ptr<Parser> new_parser(
    std::shared_ptr<flag::FlagStore> global_flag_store, const char *desc = "")
{
    return std::make_shared<Parser>(global_flag_store, desc);
}

}  // namespace impl
}  // namespace argparser
#endif

namespace argparser
{
const char* one_sentence = "Hello, here is some text without a meaning.";
const char* very_short_sentence =
    "Hello, here is some text without a meaning. This text should "
    "show what aprinted text will look like at this place.";
const char* short_sentence =
    "Hello, here is some text without a meaning. This text should "
    "show what aprinted text will look like at this place. If you read this "
    "text, you will get noinformation. Really? Is there no information? Is "
    "there a difference between thistext and some nonsense like “Huardest "
    "gefburn”? Kjift – not at all!";
const char* long_sentence =
    "Hello, here is some text without a meaning. This text should "
    "show what aprinted text will look like at this place. If you read this "
    "text, you will get noinformation. Really? Is there no information? Is "
    "there a difference between thistext and some nonsense like “Huardest "
    "gefburn”? Kjift – not at all! A blindtext like this gives you "
    "information about the selected font, how the letters arewritten and an "
    "impression of the look. This text should contain all letters ofthe "
    "alphabet and it should be written in of the original language. There is "
    "noneed for special content, but the length of words should match the "
    "language.";
const char* very_long_sentence =
    "Hello, here is some text without a meaning. This text should "
    "show what aprinted text will look like at this place. If you read this "
    "text, you will get noinformation. Really? Is there no information? Is "
    "there a difference between thistext and some nonsense like “Huardest "
    "gefburn”? Kjift – not at all! A blindtext like this gives you "
    "information about the selected font, how the letters arewritten and an "
    "impression of the look. This text should contain all letters ofthe "
    "alphabet and it should be written in of the original language. There is "
    "noneed for special content, but the length of words should match the "
    "language."
    "Hello, here is some text without a meaning. This text should "
    "show what aprinted text will look like at this place. If you read this "
    "text, you will get noinformation. Really? Is there no information? Is "
    "there a difference between thistext and some nonsense like “Huardest "
    "gefburn”? Kjift – not at all! A blindtext like this gives you "
    "information about the selected font, how the letters arewritten and an "
    "impression of the look. This text should contain all letters ofthe "
    "alphabet and it should be written in of the original language. There is "
    "noneed for special content, but the length of words should match the "
    "language.";
}  // namespace argparser

#endif