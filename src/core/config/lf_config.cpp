#include "large_flock/core/config/lf_config.hpp"

#include <iostream>

namespace large_flock {
namespace core {

void LfConfig::Configure(duckdb::DatabaseInstance &db) {
    std::string schema = "lf_config";
    duckdb::Connection con(db);
    con.BeginTransaction();

    ConfigSchema(con, schema);
    ConfigModelTable(con, schema);
    ConfigPromptTable(con, schema);

    con.Commit();
}

void LfConfig::ConfigSchema(duckdb::Connection &con, std::string &schema_name) {

    // Ensure schema exists
    auto result = con.Query("SELECT * FROM information_schema.schemata WHERE schema_name = '" + schema_name + "';");
    if (result->RowCount() == 0) {
        con.Query("CREATE SCHEMA " + schema_name + ";");
    }
}

void LfConfig::ConfigModelTable(duckdb::Connection &con, std::string &schema_name) {
    const std::string table_name = "LARGE_FLOCK_MODEL_INTERNAL_TABLE";

    // Ensure schema exists
    auto result = con.Query("SELECT table_name FROM information_schema.tables WHERE table_schema = '" + schema_name +
                            "' AND table_name = '" + table_name + "';");
    if (result->RowCount() == 0) {
        con.Query("CREATE TABLE " + schema_name + "." + table_name +
                  " ("
                  "model_name VARCHAR NOT NULL PRIMARY KEY,"
                  "model VARCHAR,"
                  "max_tokens INTEGER NOT NULL"
                  ");");

        con.Query("INSERT INTO " + schema_name + "." + table_name +
                  " (model_name, model, max_tokens) VALUES ('default', 'gpt-3.5-turbo', 4096);");
    }
}

void LfConfig::ConfigPromptTable(duckdb::Connection &con, std::string &schema_name) {
    const std::string table_name = "LARGE_FLOCK_PROMPT_INTERNAL_TABLE";

    auto result = con.Query("SELECT table_name FROM information_schema.tables WHERE table_schema = '" + schema_name +
                            "' AND table_name = '" + table_name + "';");
    if (result->RowCount() == 0) {
        con.Query("CREATE TABLE " + schema_name + "." + table_name +
                  " ("
                  "prompt_name VARCHAR NOT NULL PRIMARY KEY,"
                  "prompt VARCHAR NOT NULL"
                  ");");

        con.Query("INSERT INTO " + schema_name + "." + table_name +
                  " (prompt_name ,prompt) VALUES ('summarize', 'Summarize the following text: {{text}}');");
    }
}

} // namespace core
} // namespace large_flock
