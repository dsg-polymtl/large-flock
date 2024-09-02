#ifndef LARGE_FLOCK_CONFIG_LF_CONFIG_HPP
#define LARGE_FLOCK_CONFIG_LF_CONFIG_HPP

#include "duckdb/main/connection.hpp"
#include "duckdb/main/database.hpp"

namespace large_flock {
namespace core {

class LfConfig {
public:
    static void Configure(duckdb::DatabaseInstance &db);

private:
    static void ConfigSchema(duckdb::Connection &con, std::string &schema_name);
    static void ConfigModelTable(duckdb::Connection &con, std::string &schema_name);
    static void ConfigPromptTable(duckdb::Connection &con, std::string &schema_name);
};

} // namespace config
} // namespace large_flock

#endif // LARGE_FLOCK_CONFIG_LF_CONFIG_HPP
