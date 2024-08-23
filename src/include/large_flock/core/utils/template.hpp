#pragma once
#include "large_flock/common.hpp"
#include <nlohmann/json.hpp>

namespace large_flock
{
namespace core
{

struct TemplateUtils
{
	// get the placeholders strings from the template
	static std::vector<std::string> GetPlaceholders(const std::string &template_string);

	// construct the prompts from the template and the values
	static nlohmann::json ConstructPrompts(DataChunk& args, const nlohmann::json& args_idx);

	// generate the combined prompt
	static std::string GenerateCombinedPrompt(const nlohmann::json& context);
};

} // namespace core

} // namespace large_flock