#include <large_flock/core/utils/template.hpp>
#include <nlohmann/json.hpp>
#include <inja/inja.hpp>

namespace large_flock
{

namespace core
{

std::vector<std::string> TemplateUtils::GetPlaceholders(const std::string &template_string)
{
	std::vector<std::string> placeholders;
	std::string::size_type start = 0;
	while (start < template_string.size())
	{
		start = template_string.find("{{", start);
		if (start == std::string::npos)
		{
			break;
		}
		start += 2;
		std::string::size_type end = template_string.find("}}", start);
		if (end == std::string::npos)
		{
			break;
		}
		placeholders.push_back(template_string.substr(start, end - start));
		start = end + 2;
	}
	return placeholders;
}

nlohmann::json TemplateUtils::ConstructPrompts(DataChunk& args, const nlohmann::json& args_idx)
{	
	// get the template string
	std::string template_str = args.data[args_idx["template"]].GetValue(0).ToString();

	// construct the prompts
	inja::Environment env;
	nlohmann::json context;
	context["prompts"] = nlohmann::json::array();
	for (idx_t i=0; i<args.size(); i++)
	{
		// get the placeholders strings from the template
		std::vector<std::string> placeholders = TemplateUtils::GetPlaceholders(template_str);
		nlohmann::json params;

		for (auto &placeholder : placeholders) {
			// get the value of the placeholder
			std::string value = args.data[args_idx["inputs"][placeholder]].GetValue(i).ToString();
			params[placeholder] = value;
		}

		// render the template
		nlohmann::json prompt_context;
		prompt_context["number"] = i + 1;
		prompt_context["prompt"] = env.render(template_str, params);
		context["prompts"].push_back(prompt_context);
	}

	return context;
}

std::string TemplateUtils::GenerateCombinedPrompt(const nlohmann::json& context)
{
	inja::Environment env;
	inja::Template prompt_template = env.parse_template("src/templates/llm_map_template.txt");

	return env.render(prompt_template, context);
}
	
} // namespace core

} // namespace large_flock