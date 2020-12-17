#ifndef FLAG_STORE_H
#define FLAG_STORE_H
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "./common.hpp"
namespace argparser
{
namespace flag
{
class FlagStoreCell
{
public:
    using Pointer = std::shared_ptr<FlagStoreCell>;
    const std::string &raw() const
    {
        return raw_;
    }
    std::string &raw()
    {
        return raw_;
    }

private:
    std::string raw_;
};
class FlagStore
{
public:
    using Pointer = std::shared_ptr<FlagStore>;
    FlagStore() = default;
    static Pointer global_instance()
    {
        static Pointer ptr;
        if (ptr == nullptr)
        {
            ptr = std::make_shared<FlagStore>();
        }
        return ptr;
    }
    Pointer new_instance()
    {
        return std::make_shared<FlagStore>();
    }

    bool has(const std::string &name) const
    {
        return get(name) != nullptr;
    }
    FlagStoreCell::Pointer get(const std::string &name) const
    {
        if (flag::is_full_flag(name))
        {
            auto it = long_name_index_.find(name);
            if (it != long_name_index_.end())
            {
                return it->second;
            }
            return nullptr;
        }
        if (flag::is_short_flag(name))
        {
            auto it = short_name_index_.find(name);
            if (it != short_name_index_.end())
            {
                return it->second;
            }
            return nullptr;
        }
        return nullptr;
    }

    FlagStoreCell::Pointer reg(const std::string &full_name,
                               const std::string &short_name)
    {
        if (flag::is_full_flag(full_name))
        {
            flags_.push_back(std::make_shared<FlagStoreCell>());
            long_name_index_[full_name] = flags_.back();
            if (flag::is_short_flag(short_name))
            {
                short_name_index_[short_name] = flags_.back();
            }
            return flags_.back();
        }
        else if (flag::is_short_flag(short_name))
        {
            flags_.push_back(std::make_shared<FlagStoreCell>());
            short_name_index_[short_name] = std::make_shared<FlagStoreCell>();
            return flags_.back();
        }
        else
        {
            return nullptr;
        }
    }

private:
    std::unordered_map<std::string, FlagStoreCell::Pointer> long_name_index_;
    std::unordered_map<std::string, FlagStoreCell::Pointer> short_name_index_;
    std::vector<FlagStoreCell::Pointer> flags_;
};
}  // namespace flag
}  // namespace argparser
#endif