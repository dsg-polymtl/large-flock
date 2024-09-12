#include <inja/inja.hpp>
#include <large_flock/common.hpp>
#include <large_flock/core/functions/scalar.hpp>
#include <large_flock/core/model_manager/model_manager.hpp>
#include <large_flock/core/parser/llm_response.hpp>
#include <large_flock/core/parser/scalar.hpp>
#include <large_flock_extension.hpp>
#include <nlohmann/json.hpp>

namespace large_flock {
namespace core {

inline std::string ConstructPrompt(DataChunk &args, Connection &con) {
    inja::Environment env;

    auto prompt_name = args.data[0].GetValue(0).ToString();

    auto query_result = con.Query(
        "SELECT prompt FROM lf_config.LARGE_FLOCK_PROMPT_INTERNAL_TABLE WHERE prompt_name = '" + prompt_name + "'");

    if (query_result->RowCount() == 0) {
        throw std::runtime_error("Prompt not found");
    }

    auto template_str = query_result->GetValue(0, 0).ToString();

    std::string filled_template = "";
    auto params = CoreScalarParsers::Struct2Json(args.data[2], args.size());
    for (idx_t i = 0; i < args.size(); i++) {
        filled_template += "Prompt " + std::to_string(i + 1) + ": " + env.render(template_str, params[i]) + "\n\n";
    }

    std::string prompt = env.render_file("src/templates/lf_map/prompt_template.txt", {{"prompts", filled_template}});

    return prompt;
}

static void LfMapScalarFunction(DataChunk &args, ExpressionState &state, Vector &result) {
    // parse the arguments and return the args key idx pair
    Connection con(*state.GetContext().db);
    CoreScalarParsers::LfMapScalarParser(args);
    auto model = args.data[1].GetValue(0).ToString();
    auto prompt = ConstructPrompt(args, con);
    nlohmann::json settings;
    if (args.ColumnCount() == 4) {
        settings = CoreScalarParsers::Struct2Json(args.data[3], args.size())[0];
    }

    auto query_result =
        con.Query("SELECT model FROM lf_config.LARGE_FLOCK_MODEL_INTERNAL_TABLE WHERE model_name = '" + model + "'");
    if (query_result->RowCount() == 0) {
        throw std::runtime_error("Model not found");
    }
    auto model_name = query_result->GetValue(0, 0).ToString();

    auto llm_response = ModelManager::CallComplete(prompt, model_name, settings);

    // set the result
    auto index = 0;
    Vector vec(LogicalType::VARCHAR, args.size());
    UnaryExecutor::Execute<string_t, string_t>(vec, result, args.size(), [&](string_t _) {
        return StringVector::AddString(result, llm_response["Prompt " + std::to_string(index++ + 1)].dump());
    });
}

//------------------------------------------------------------------------------
// Register functions
//------------------------------------------------------------------------------
void CoreScalarFunctions::RegisterLfMapScalarFunction(DatabaseInstance &db) {
    ExtensionUtil::RegisterFunction(db, ScalarFunction("lf_map", {}, LogicalType::VARCHAR, LfMapScalarFunction, nullptr,
                                                       nullptr, nullptr, nullptr, LogicalType::ANY));
}

} // namespace core
} // namespace large_flock
