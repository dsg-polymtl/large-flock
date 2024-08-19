#define DUCKDB_EXTENSION_MAIN

#include "llm_extension.hpp"
#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/function/scalar_function.hpp"
#include "duckdb/main/extension_util.hpp"
#include "duckdb/parser/parsed_data/create_scalar_function_info.hpp"
#include "utils/validation_utils.h"
#include "utils/template_utils.h"
#include "utils/response_utils.h"
#include "utils/litellm.h"
namespace duckdb
{

    inline void LlmScalarFun(DataChunk &args, ExpressionState &state, Vector &result)
    {
        auto &prompt_vector = args.data[0];
        UnaryExecutor::Execute<string_t, string_t>(
            prompt_vector, result, args.size(),
            [&](string_t prompt)
            {
                std::string response_text = litellm(prompt.GetString());
                return StringVector::AddString(result, response_text);
            });
    }

    inline void LlmMapScalarFun(DataChunk &args, ExpressionState &state, Vector &result)
    {
        InputsValidator(args);

        std::vector<std::string> all_responses = ChunkAndSendRequests(args);

        size_t index = 0;
        UnaryExecutor::Execute<string_t, string_t>(
            args.data[1], result, args.size(),
            [&](string_t _)
            {
                std::string response = all_responses[index];
                index++;
                return StringVector::AddString(result, response);
            });
    }

    inline void LlmSummarizeScalarFun(DataChunk &args, ExpressionState &state, Vector &result)
    {
        // Initialize a template string for summarization
        std::string summarize_template = "summarize these data: ";
        for (idx_t i = 0; i < args.size(); ++i)
        {
            summarize_template += "{{}} ";
        }

        // Prepare a new DataChunk
        DataChunk new_chunk;
        vector<LogicalType> types = {LogicalType::VARCHAR};
        for (idx_t i = 0; i < args.ColumnCount(); i++)
        {
            types.push_back(args.data[i].GetType());
        }
        Allocator allocator; // Create an allocator instance
        new_chunk.Initialize(allocator, types);
        new_chunk.SetCardinality(args.size() + 1);

        // Set the summarization template in the first column
        for (idx_t i = 0; i < args.size(); i++)
        {
            new_chunk.SetValue(0, i, summarize_template);

            for (idx_t j = 1; j < args.ColumnCount() + 1; j++)
            {
                new_chunk.SetValue(j, i, args.data[j - 1].GetValue(i).ToString());
            }
        }
        // Call LlmMapScalarFun using the new chunk
        LlmMapScalarFun(new_chunk, state, result);
    }

    static void LoadInternal(DatabaseInstance &instance)
    {
        // Register a scalar function
        auto llm_scalar_function = ScalarFunction("llm", {LogicalType::VARCHAR}, LogicalType::VARCHAR, LlmScalarFun);
        ExtensionUtil::RegisterFunction(instance, llm_scalar_function);

        auto llm_map_scalar_function = ScalarFunction("llm_map", {LogicalType::VARCHAR}, LogicalType::VARCHAR, LlmMapScalarFun);
        llm_map_scalar_function.varargs = LogicalType::ANY;
        ExtensionUtil::RegisterFunction(instance, llm_map_scalar_function);

        auto llm_summarize_function = ScalarFunction("llm_summarize", {LogicalType::ANY}, LogicalType::VARCHAR, LlmSummarizeScalarFun);
        llm_summarize_function.varargs = LogicalType::ANY;
        ExtensionUtil::RegisterFunction(instance, llm_summarize_function);
    }

    void LlmExtension::Load(DuckDB &db)
    {
        LoadInternal(*db.instance);
    }

    std::string LlmExtension::Name()
    {
        return "llm";
    }

    std::string LlmExtension::Version() const
    {
#ifdef EXT_VERSION_LLM
        return EXT_VERSION_LLM;
#else
        return "";
#endif
    }

} // namespace duckdb

extern "C"
{

    DUCKDB_EXTENSION_API void llm_init(duckdb::DatabaseInstance &db)
    {
        duckdb::DuckDB db_wrapper(db);
        db_wrapper.LoadExtension<duckdb::LlmExtension>();
    }

    DUCKDB_EXTENSION_API const char *llm_version()
    {
        return duckdb::DuckDB::LibraryVersion();
    }
}

#ifndef DUCKDB_EXTENSION_MAIN
#error DUCKDB_EXTENSION_MAIN not defined
#endif
