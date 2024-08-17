#define DUCKDB_EXTENSION_MAIN

#include "llm_extension.hpp"
#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/function/scalar_function.hpp"
#include "duckdb/main/extension_util.hpp"
#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>

#include <utils/utils.h>

// #include <inja.hpp>

namespace duckdb
{

    inline void LlmScalarFun(DataChunk &args, ExpressionState &state, Vector &result)
    {
        auto &prompt_vector = args.data[0];
        UnaryExecutor::Execute<string_t, string_t>(
            prompt_vector, result, args.size(),
            [&](string_t prompt)
            {
                std::string response_text = GetOpenAIResponse(prompt.GetString());
                return StringVector::AddString(result, response_text);
            });
    }

    inline void LlmMapScalarFun(DataChunk &args, ExpressionState &state, Vector &result)
    {
        if (args.ColumnCount() < 2)
        {
            throw InvalidInputException("llm_map requires at least two arguments: the template and at least one data column.");
        }

        // The first argument should be the prompt template (string)
        if (args.data[0].GetType() != LogicalType::VARCHAR)
        {
            throw InvalidInputException("The first argument must be a prompt template.");
        }

        auto &template_vector = args.data[0];
        std::vector<std::reference_wrapper<Vector>> args_vector;

        for (size_t i = 1; i < args.ColumnCount(); ++i)
        {
            args_vector.push_back(std::ref(args.data[i]));
        }

        // Construct the combined prompt
        std::string combined_prompt;
        combined_prompt = "You are an AI assistant. You have been asked to provide responses to the following prompts respectively:\n\n";
        for (size_t i = 0; i < args.size(); ++i)
        {
            std::string template_str = template_vector.GetValue(i).ToString();
            std::vector<std::string> params;
            for (auto &arg_ref : args_vector)
            {
                Vector &arg = arg_ref.get();
                auto value = arg.GetValue(i).ToString();
                params.push_back(value);
            }
            combined_prompt += "Prompt " + std::to_string(i + 1) + ": " + replace_placeholders(template_str, params) + "\n";
        }

        // Send the combined prompt to OpenAI API
        std::string response_text;
        try
        {
            response_text = GetOpenAIResponse(combined_prompt); // Function to call OpenAI API
        }
        catch (const std::exception &e)
        {
            throw InvalidInputException("Failed to get response from OpenAI API: " + std::string(e.what()));
        }

        // Parse the response
        std::vector<std::string> parsed_responses = ParseApiResponse(response_text, args.size());

        // Store the results
        size_t index = 0;
        UnaryExecutor::Execute<string_t, string_t>(
            args_vector[0], result, args.size(),
            [&](string_t _)
            {
                std::string response = parsed_responses[index];
                index++;
                return StringVector::AddString(result, response);
            });
    }

    inline void LlmSummarizeScalarFun(DataChunk &args, ExpressionState &state, Vector &result)
    {
        // Initialize a template string for summarization
        string summarize_template = "summarize this sentence: {{}}";
        auto &text_vector = args.data[0];

        // Prepare a new DataChunk
        DataChunk new_chunk;
        vector<LogicalType> types = {LogicalType::VARCHAR, LogicalType::VARCHAR};
        Allocator allocator; // Create an allocator instance
        new_chunk.Initialize(allocator, types);

        // Set the summarization template in the first column
        for (idx_t i = 0; i < args.size(); i++)
        {
            new_chunk.SetValue(0, i, summarize_template);
            new_chunk.SetValue(1, i, text_vector.GetValue(i).ToString());
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

        auto llm_summarize_function = ScalarFunction("llm_summarize", {LogicalType::VARCHAR}, LogicalType::VARCHAR, LlmSummarizeScalarFun);
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
