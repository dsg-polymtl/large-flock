#include <large_flock_extension.hpp>
#include <large_flock/common.hpp>
#include <large_flock/core/functions/scalar.hpp>
#include <large_flock/core/parser/scalar.hpp>
#include <large_flock/core/parser/llm_response.hpp>
#include <large_flock/core/utils/template.hpp>
#include <large_flock/core/model_manager/model_manager.hpp>
#include <nlohmann/json.hpp>
#include <inja/inja.hpp>

namespace large_flock
{

namespace core
{

static void LlmMapScalarFunction(DataChunk &args, ExpressionState &state, Vector &result)
{
	// parse the arguments and return the args key idx pair
	nlohmann::json key_idx_pair = CoreScalarParsers::LlmMapScalarParser(args);
	// Create the prompt context
	nlohmann::json context = TemplateUtils::ConstructPrompts(args, key_idx_pair);
	// Generate the combined prompt
	std::string combined_prompt = TemplateUtils::GenerateCombinedPrompt(context);
	// Get the model name, max tokens and temperature
	std::string model_name = key_idx_pair["model"] != -1 ? args.data[key_idx_pair["model"]].GetValue(0).ToString() : "gpt-3.5-turbo";
	int max_tokens = key_idx_pair["max_tokens"] != -1 ? args.data[key_idx_pair["max_tokens"]].GetValue(0).GetValue<int>() : 100;
	float temperature = key_idx_pair["temperature"] != -1 ? args.data[key_idx_pair["temperature"]].GetValue(0).GetValue<float>() : 1.0;

	std::string llm_response = ModelManager::OpenAI(combined_prompt, model_name, max_tokens, temperature);

	// parse the llm response
	std::vector<std::string> response_list = CoreLlmResponseParsers::LlmMapResponseParser(llm_response, args.size());

	// set the result
	size_t index = 0;
	Vector vec(LogicalType::VARCHAR, args.size());
	UnaryExecutor::Execute<string_t, string_t>(
		vec, result, args.size(),
		[&](string_t _)
		{
			std::string response = response_list[index++];
			return StringVector::AddString(result, response);
		});
}

//------------------------------------------------------------------------------
// Register functions
//------------------------------------------------------------------------------
void CoreScalarFunctions::RegisterLlmMapScalarFunction(DatabaseInstance& db)
{
	ExtensionUtil::RegisterFunction(
		db,
		ScalarFunction("llm_map", {}, LogicalType::VARCHAR, LlmMapScalarFunction, nullptr, nullptr, nullptr, nullptr, LogicalType::ANY));
}

} // namespace core

} // namespace large_flock
