#ifndef FLAG_VALIDATOR_H
#define FLAG_VALIDATOR_H
#include <iostream>
#include <set>
#include <string>

#include "./common.hpp"
#include "./flag-manager.hpp"
namespace argparser
{
namespace flag
{
class Validator
{
public:
    Validator(flag::FlagManager::Pointer gf_manager) : gf_manager_(gf_manager)
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
        if (gf_manager_->contain(full_name) || gf_manager_->contain(short_name))
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
    flag::FlagManager::Pointer gf_manager_;
};
}  // namespace flag
}  // namespace argparser
#endif