#define DUCKDB_EXTENSION_MAIN

#include "llm_extension.hpp"
#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/function/scalar_function.hpp"
#include "duckdb/main/extension_util.hpp"
#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>

#include "openai.hpp"

namespace duckdb {

inline void LlmScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
    auto &prompt_vector = args.data[0];
    UnaryExecutor::Execute<string_t, string_t>(
        prompt_vector, result, args.size(),
        [&](string_t prompt) {
            // Start OpenAI with the environment variable API key
            openai::start(); 
            // Create a JSON request payload
            nlohmann::json request_payload = {
                {"model", "gpt-3.5-turbo-instruct"},
                {"prompt", prompt.GetString()},
                {"max_tokens", 15},
                {"temperature", 0.7}
            };
            // Make a request to the OpenAI API
            auto completion = openai::completion().create(request_payload);
            // Extract the text from the response
            std::string response_text = completion["choices"][0]["text"].get<std::string>();
            // Trim leading newlines
            response_text.erase(0, response_text.find_first_not_of("\n"));
            // Return the response text as the result
            return StringVector::AddString(result, response_text);
        }
    );
}

static void LoadInternal(DatabaseInstance &instance) {
    // Register a scalar function
    auto llm_scalar_function = ScalarFunction("llm", {LogicalType::VARCHAR}, LogicalType::VARCHAR, LlmScalarFun);
    ExtensionUtil::RegisterFunction(instance, llm_scalar_function);
}

void LlmExtension::Load(DuckDB &db) {
	LoadInternal(*db.instance);
}
std::string LlmExtension::Name() {
	return "llm";
}

std::string LlmExtension::Version() const {
#ifdef EXT_VERSION_LLM
	return EXT_VERSION_LLM;
#else
	return "";
#endif
}

} // namespace duckdb

extern "C" {

DUCKDB_EXTENSION_API void llm_init(duckdb::DatabaseInstance &db) {
    duckdb::DuckDB db_wrapper(db);
    db_wrapper.LoadExtension<duckdb::LlmExtension>();
}

DUCKDB_EXTENSION_API const char *llm_version() {
	return duckdb::DuckDB::LibraryVersion();
}
}

#ifndef DUCKDB_EXTENSION_MAIN
#error DUCKDB_EXTENSION_MAIN not defined
#endif
