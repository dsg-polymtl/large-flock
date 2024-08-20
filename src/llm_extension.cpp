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
#include <unordered_map>
#include <vector>
#include <functional>

// Define a type alias for convenience
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

        KeyValueMap data_map = ParseKeyValuePairs(args);

        size_t size = args.size();

        std::vector<std::string> all_responses = ChunkAndSendRequests(data_map, size);

        size_t index = 0;
        UnaryExecutor::Execute<string_t, string_t>(
            args.data[args.ColumnCount() - 1], result, size,
            [&](string_t _)
            {
                std::string response = all_responses[index];
                index++;
                return StringVector::AddString(result, response);
            });
    }

    inline void SetLlmConfiguration(DataChunk &args, ExpressionState &state, Vector &result)
    {
        KeyValueMap data_map = ParseKeyValuePairs(args);

        for (const auto &pair : data_map)
        {
            const std::string &setting = pair.first;
            const auto &value_ptr = pair.second;

            if (setting == "default_model")
            {
                LlmExtension::SetDefaultModel(value_ptr->GetValue(0).ToString());
            }
            else if (setting == "context_window")
            {
                LlmExtension::SetContextWindow(value_ptr->GetValue(0).GetValue<int>());
            }
            else if (setting == "temperature")
            {
                LlmExtension::SetTemperature(value_ptr->GetValue(0).GetValue<double>());
            }
            else
            {
                throw InvalidInputException("Unknown setting key: " + setting);
            }
        }
        duckdb::Vector vec(LogicalType::VARCHAR, 1);
        UnaryExecutor::Execute<string_t, string_t>(
            vec, result, 1,
            [&](string_t _)
            {
                return StringVector::AddString(result, "Config successfully updated");
            });
    }

    inline void GetLlmConfiguration(DataChunk &args, ExpressionState &state, Vector &result)
    {
        // Create a formatted string containing all the configuration values
        std::string config = "Default Model: " + LlmExtension::GetDefaultModel() + " | ";
        config += "Context Window: " + std::to_string(LlmExtension::GetContextWindow()) + " | ";
        config += "Temperature: " + std::to_string(LlmExtension::GetTemperature());

        duckdb::Vector vec(LogicalType::VARCHAR, 1);
        UnaryExecutor::Execute<string_t, string_t>(
            vec, result, 1,
            [&](string_t _)
            {
                return StringVector::AddString(result, config);
            });
    }

    inline void LlmSummarizeScalarFun(DataChunk &args, ExpressionState &state, Vector &result)
    {
        KeyValueMap data_map = ParseKeyValuePairs(args);

        // Initialize a template string for summarization
        std::string summarize_template = "write a short summary of the next data:\n";

        for (const auto &pair : data_map)
        {
            const std::string &key = pair.first;

            summarize_template += "{{" + key + "}}\n";
        }

        // Prepare a new DataChunk
        DataChunk new_chunk;
        vector<LogicalType> types = {LogicalType::VARCHAR, LogicalType::VARCHAR};
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
            new_chunk.SetValue(0, i, "template");
            new_chunk.SetValue(1, i, summarize_template);

            for (idx_t j = 0; j < args.ColumnCount(); j++)
            {
                new_chunk.SetValue(j+2, i, args.data[j].GetValue(i).ToString());
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

        auto llm_map_scalar_function = ScalarFunction("llm_map", {LogicalType::VARCHAR, LogicalType::VARCHAR}, LogicalType::VARCHAR, LlmMapScalarFun);
        llm_map_scalar_function.varargs = LogicalType::ANY;
        ExtensionUtil::RegisterFunction(instance, llm_map_scalar_function);

        auto llm_summarize_function = ScalarFunction("llm_summarize", {LogicalType::ANY, LogicalType::ANY}, LogicalType::VARCHAR, LlmSummarizeScalarFun);
        llm_summarize_function.varargs = LogicalType::ANY;
        ExtensionUtil::RegisterFunction(instance, llm_summarize_function);

        auto set_llm_config = ScalarFunction("set_llm_config", {LogicalType::VARCHAR, LogicalType::VARCHAR}, LogicalType::VARCHAR, SetLlmConfiguration);
        set_llm_config.varargs = LogicalType::ANY;
        ExtensionUtil::RegisterFunction(instance, set_llm_config);

        auto get_llm_config = ScalarFunction("get_llm_config", {}, LogicalType::VARCHAR, GetLlmConfiguration);
        ExtensionUtil::RegisterFunction(instance, get_llm_config);
    }

    std::string LlmExtension::default_model = "gpt-3.5-turbo-instruct";
    int LlmExtension::context_window = 4191;
    double LlmExtension::temperature = 1.0;

    // Implementation of static methods

    void LlmExtension::SetDefaultModel(const std::string &model_name)
    {
        LlmExtension::default_model = model_name;
    }

    void LlmExtension::SetContextWindow(int window)
    {
        LlmExtension::context_window = window;
    }

    void LlmExtension::SetTemperature(double temp)
    {
        LlmExtension::temperature = temp;
    }

    std::string LlmExtension::GetDefaultModel()
    {
        return LlmExtension::default_model;
    }

    int LlmExtension::GetContextWindow()
    {
        return LlmExtension::context_window;
    }

    double LlmExtension::GetTemperature()
    {
        return LlmExtension::temperature;
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
