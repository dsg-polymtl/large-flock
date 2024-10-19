#define DUCKDB_EXTENSION_MAIN

#include "large_flock_extension.hpp"

#include "duckdb/parser/parser.hpp"
#include "duckdb/parser/statement/extension_statement.hpp"
#include "large_flock/common.hpp"
#include "large_flock/core/module.hpp"
#include "large_flock/core/parser/lf_query_parser.hpp"

namespace duckdb {

static void LoadInternal(DatabaseInstance &instance) {
    large_flock::core::CoreModule::Register(instance);

    // Register the custom parser
    auto &config = DBConfig::GetConfig(instance);
    DuckParserExtension duck_parser;
    config.parser_extensions.push_back(duck_parser);
    config.operator_extensions.push_back(make_uniq<DuckOperatorExtension>());
}

ParserExtensionParseResult duck_parse(ParserExtensionInfo *, const std::string &query) {
    large_flock::core::LfQueryParser lf_query_parser;

    // Translate and print SQL queries for each input query
    std::string sql_query = lf_query_parser.ParseQuery(query);

    // Parse and return the statement using DuckDB's parser
    Parser parser;
    parser.ParseQuery(sql_query);
    auto statements = std::move(parser.statements);

    return ParserExtensionParseResult(
        make_uniq_base<ParserExtensionParseData, DuckParseData>(std::move(statements[0])));
}

ParserExtensionPlanResult duck_plan(ParserExtensionInfo *, ClientContext &context,
                                    unique_ptr<ParserExtensionParseData> parse_data) {
    auto duck_state = make_shared_ptr<DuckState>(std::move(parse_data));
    context.registered_state["duck"] = duck_state;
    throw BinderException("Use duck_bind instead");
}

BoundStatement duck_bind(ClientContext &context, Binder &binder, OperatorExtensionInfo *info, SQLStatement &statement) {
    switch (statement.type) {
    case StatementType::EXTENSION_STATEMENT: {
        auto &extension_statement = dynamic_cast<ExtensionStatement &>(statement);
        if (extension_statement.extension.parse_function == duck_parse) {
            auto lookup = context.registered_state.find("duck");
            if (lookup != context.registered_state.end()) {
                auto duck_state = (DuckState *)lookup->second.get();
                auto duck_binder = Binder::CreateBinder(context, &binder);
                auto duck_parse_data = dynamic_cast<DuckParseData *>(duck_state->parse_data.get());
                auto statement = duck_binder->Bind(*(duck_parse_data->statement));
                BoundStatement result;
                return statement;
            }
            throw BinderException("Registered state not found");
        }
    }
    default:
        // No-op empty
        return {};
    }
}

void LargeFlockExtension::Load(DuckDB &db) {
    LoadInternal(*db.instance);
}

std::string LargeFlockExtension::Name() {
    return "large_flock";
}
std::string LargeFlockExtension::Version() const {
#ifdef EXT_VERSION_LARGE_FLOCK
    return EXT_VERSION_LARGE_FLOCK;
#else
    return "";
#endif
}

} // namespace duckdb

extern "C" {

DUCKDB_EXTENSION_API void large_flock_init(DatabaseInstance &db) {
    duckdb::DuckDB db_wrapper(db);
    db_wrapper.LoadExtension<duckdb::LargeFlockExtension>();
}

DUCKDB_EXTENSION_API const char *large_flock_version() {
    return duckdb::DuckDB::LibraryVersion();
}
}

#ifndef DUCKDB_EXTENSION_MAIN
#error DUCKDB_EXTENSION_MAIN not defined
#endif
