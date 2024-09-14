#include <algorithm>
#include <cmath>
#include <fstream>
#include <inja/inja.hpp>
#include <iostream>
#include <large_flock/common.hpp>
#include <large_flock/core/functions/scalar.hpp>
#include <large_flock/core/model_manager/model_manager.hpp>
#include <large_flock/core/model_manager/openai.hpp>
#include <large_flock/core/parser/llm_response.hpp>
#include <large_flock/core/parser/scalar.hpp>
#include <large_flock_extension.hpp>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

namespace large_flock {
namespace core {

inline int GetNumTokens(const std::string &str) {
    nlohmann::json request_payload = {{"model", "text-embedding-3-small"}, {"input", str}};
    auto num_tokens = openai::embedding().create(request_payload)["usage"]["prompt_tokens"];
    return num_tokens;
}

template <typename T>
std::string to_string(const T &value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

nlohmann::json get_max_length_values(const std::vector<nlohmann::json> &params) {
    nlohmann::json result;

    for (const auto &json_obj : params) {
        for (const auto &item : json_obj.items()) {
            std::string key = item.key();
            std::string value_str = to_string(item.value());
            int length = value_str.length();

            if (result.contains(key)) {
                std::string current_max_value_str = to_string(result[key]);
                if (current_max_value_str.length() < length) {
                    result[key] = item.value();
                }
            } else {
                result[key] = item.value();
            }
        }
    }

    return result;
}

std::string read_file_to_string(const std::string &file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open the file: " + file_path);
    }

    std::ostringstream content_stream;
    content_stream << file.rdbuf();
    return content_stream.str();
}

std::string combine_values(const nlohmann::json &json_obj) {
    std::string combined;
    for (const auto &item : json_obj.items()) {
        combined += to_string(item.value()) + " ";
    }

    if (!combined.empty()) {
        combined.pop_back();
    }
    return combined;
}

inline std::vector<std::string> ConstructPrompts(DataChunk &args, Connection &con, int model_max_tokens = 4096) {
    inja::Environment env;

    auto prompt_name = args.data[0].GetValue(0).ToString();
    auto query_result = con.Query(
        "SELECT prompt FROM lf_config.LARGE_FLOCK_PROMPT_INTERNAL_TABLE WHERE prompt_name = '" + prompt_name + "'");

    if (query_result->RowCount() == 0) {
        throw std::runtime_error("Prompt not found");
    }

    auto template_str = query_result->GetValue(0, 0).ToString();
    auto row_tokens = GetNumTokens(template_str);

    auto params = CoreScalarParsers::Struct2Json(args.data[2], args.size());
    auto max_length_values = get_max_length_values(params);
    auto combined_values = combine_values(max_length_values);
    row_tokens += GetNumTokens(combined_values);

    std::vector<std::string> prompts;

    if (row_tokens > model_max_tokens) {
        throw std::runtime_error("The total number of tokens in the prompt exceeds the model's maximum token limit");
    } else {
        auto template_tokens = GetNumTokens(read_file_to_string("src/templates/lf_map/prompt_template.txt"));
        auto max_tokens_for_rows = model_max_tokens - template_tokens;
        auto max_chunk_size = max_tokens_for_rows / row_tokens;
        auto chunk_size = std::min(max_chunk_size, static_cast<int>(args.size()));
        auto num_chunks = static_cast<int>(std::ceil(static_cast<double>(args.size()) / chunk_size));

        for (int i = 0; i < num_chunks; ++i) {
            nlohmann::json data;
            data["prompts"] = template_str;

            for (int j = 0; j < chunk_size; ++j) {
                data["rows"].push_back(params[i + j]);
            }

            std::string prompt = env.render_file("src/templates/lf_map/prompt_template.txt", data);
            prompts.push_back(prompt);
        }
    }

    return prompts;
}

static void LfMapScalarFunction(DataChunk &args, ExpressionState &state, Vector &result) {
    Connection con(*state.GetContext().db);
    CoreScalarParsers::LfMapScalarParser(args);

    auto model = args.data[1].GetValue(0).ToString();
    auto query_result = con.Query(
        "SELECT model, max_tokens FROM lf_config.LARGE_FLOCK_MODEL_INTERNAL_TABLE WHERE model_name = '" + model + "'");

    if (query_result->RowCount() == 0) {
        throw std::runtime_error("Model not found");
    }

    auto model_name = query_result->GetValue(0, 0).ToString();
    auto model_max_tokens = query_result->GetValue(1, 0).GetValue<int32_t>();

    auto prompts = ConstructPrompts(args, con, model_max_tokens);

    nlohmann::json settings;
    if (args.ColumnCount() == 4) {
        settings = CoreScalarParsers::Struct2Json(args.data[3], args.size())[0];
    }

    nlohmann::json rows = nlohmann::json::array();
    for (const auto &prompt : prompts) {
        // Call ModelManager::CallComplete and get the rows
        auto result = ModelManager::CallComplete(prompt, model_name, settings);

        // Check if the result contains the 'rows' field and push it to the main 'rows'
        if (result.contains("rows")) {
            for (const auto &row : result["rows"]) {
                rows.push_back(row);
            }
        }
    }

    auto index = 0;
    Vector vec(LogicalType::VARCHAR, args.size());
    UnaryExecutor::Execute<string_t, string_t>(
        vec, result, args.size(), [&](string_t _) { return StringVector::AddString(result, rows[index++].dump()); });
}

void CoreScalarFunctions::RegisterLfMapScalarFunction(DatabaseInstance &db) {
    ExtensionUtil::RegisterFunction(db, ScalarFunction("lf_map", {}, LogicalType::VARCHAR, LfMapScalarFunction, nullptr,
                                                       nullptr, nullptr, nullptr, LogicalType::ANY));
}

} // namespace core
} // namespace large_flock
