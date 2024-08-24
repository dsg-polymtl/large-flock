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

inline std::string ConstructPrompt(DataChunk &args, const nlohmann::json &args_idx) {
	inja::Environment env;

	// get the template string
	auto template_str = args.data[args_idx["template"]].GetValue(0).ToString();

	std::string filled_template = "";
	for (idx_t i = 0; i < args.size(); i++) {
		nlohmann::json *params = new nlohmann::json;

		// get the inputs from the args and store in the params
		for (const auto &el : args_idx["inputs"].items()) {
			std::string key = el.key();
			idx_t idx = el.value();
			(*params)[key] = args.data[idx].GetValue(i).ToString();
		}

		// render the template string with the params
		filled_template += "Prompt " + std::to_string(i + 1) + ": " + env.render(template_str, (*params)) + "\n\n";
		delete params;
	}

	std::string prompt = env.render_file("src/templates/lf_map/prompt_template.txt", {{"prompts", filled_template}});

	return prompt;
}

static void LfMapScalarFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	// parse the arguments and return the args key idx pair
	auto key_idx_pair = CoreScalarParsers::LfMapScalarParser(args);
	auto prompt = ConstructPrompt(args, key_idx_pair);

	// Get the model name, max tokens and temperature
	auto model_name = args.data[key_idx_pair["model"]].GetValue(0).ToString();
	auto max_tokens =
	    key_idx_pair["max_tokens"] != -1 ? args.data[key_idx_pair["max_tokens"]].GetValue(0).GetValue<int>() : 100;
	auto temperature =
	    key_idx_pair["temperature"] != -1 ? args.data[key_idx_pair["temperature"]].GetValue(0).GetValue<float>() : 1.0;

	std::string llm_response = ModelManager::CallComplete(prompt, model_name, max_tokens, temperature);

	// parse the llm response
	std::vector<std::string> response_list = CoreLlmResponseParsers::LfMapResponseParser(llm_response, args.size());

	// set the result
	auto index = 0;
	Vector vec(LogicalType::VARCHAR, args.size());
	UnaryExecutor::Execute<string_t, string_t>(vec, result, args.size(), [&](string_t _) {
		std::string response = response_list[index++];
		return StringVector::AddString(result, response);
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
