#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <toml.hpp>

toml::value* getTable(const std::filesystem::path& path);

#endif // RESOURCE_MANAGER_HPP
