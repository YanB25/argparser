#ifndef ARG_PARSER_MANAGER_H
#define ARG_PARSER_MANAGER_H

#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "./common.hpp"
#include "./flag.hpp"

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
            std::cout << std::endl;
        }
        for (const auto &[flag, meta] : allocated_flags_)
        {
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