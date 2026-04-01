#include <map>
#include <set>
#include "resource_manager.hpp"
#include "error.hpp"

static std::map<std::string, toml::value> g_tables;
static std::set<std::string> g_tables_blacklist;

toml::value* getTable(const std::filesystem::path& path) {
    if(g_tables_blacklist.contains(path)) {
        return nullptr;
    }
    if(!g_tables.contains(path)) {
        try {
            g_tables[path] = toml::parse(path);
        } catch(const std::exception& e) {
            showErrorDialog(QString::fromStdString("Ошибка парсинга " + path.string() + ":\n" + e.what()));
            g_tables_blacklist.insert(path);
            return nullptr;
        }
    }
    return &g_tables[path];
}
