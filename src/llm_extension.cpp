#define DUCKDB_EXTENSION_MAIN

#include "llm_extension.hpp"
#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/function/scalar_function.hpp"
#include "duckdb/main/extension_util.hpp"
#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>

#include "openai.hpp"

// Function to get response from OpenAI
std::string GetOpenAIResponse(const std::string &prompt)
{
    // Start OpenAI with the environment variable API key
    openai::start();

    // Create a JSON request payload
    nlohmann::json request_payload = {
        {"model", "gpt-3.5-turbo-instruct"},
        {"prompt", prompt},
        {"max_tokens", 15},
        {"temperature", 0}};

    // Make a request to the OpenAI API
    auto completion = openai::completion().create(request_payload);

    // Extract the text from the response
    std::string response_text = completion["choices"][0]["text"].get<std::string>();

    // Trim leading newlines
    response_text.erase(0, response_text.find_first_not_of("\n"));

    return response_text;
}
std::string replace_placeholders(const std::string &template_str, const std::vector<std::string> &args)
{
    std::stringstream ss;
    size_t pos = 0, arg_idx = 0;

    while (pos < template_str.size() && arg_idx < args.size())
    {
        size_t start = template_str.find("{{", pos);
        if (start == std::string::npos)
        {
            ss << template_str.substr(pos);
            break;
        }
        size_t end = template_str.find("}}", start);
        if (end == std::string::npos)
        {
            ss << template_str.substr(pos);
            break;
        }

        ss << template_str.substr(pos, start - pos) << args[arg_idx++];
        pos = end + 2;
    }
    ss << template_str.substr(pos);
    return ss.str();
}

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
        auto &template_vector = args.data[0];
        std::vector<std::reference_wrapper<Vector>> args_vector;
        for (size_t i = 1; i < args.ColumnCount(); ++i)
        {
            args_vector.push_back(std::ref(args.data[i]));
        }

        size_t index = 0;

        UnaryExecutor::Execute<string_t, string_t>(
            args_vector[0], result, args.size(),
            [&](string_t _)
            {
                std::string template_str = template_vector.GetValue(index).ToString();
                std::vector<std::string> params;
                for (auto &arg_ref : args_vector)
                {
                    Vector &arg = arg_ref.get();
                    auto value = arg.GetValue(index).ToString();
                    params.push_back(value);
                }
                std::string prompt = replace_placeholders(template_str, params);

                std::string response_text = GetOpenAIResponse(prompt);

                index++;

                return StringVector::AddString(result, response_text);
            });
    }

    static void LoadInternal(DatabaseInstance &instance)
    {
        // Register a scalar function
        auto llm_scalar_function = ScalarFunction("llm", {LogicalType::VARCHAR}, LogicalType::VARCHAR, LlmScalarFun);
        ExtensionUtil::RegisterFunction(instance, llm_scalar_function);

        auto llm_map_scalar_function = ScalarFunction("llm_map", {LogicalType::VARCHAR, LogicalType::VARCHAR}, LogicalType::VARCHAR, LlmMapScalarFun);
        ExtensionUtil::RegisterFunction(instance, llm_map_scalar_function);
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
