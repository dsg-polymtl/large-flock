#include <large_flock_extension.hpp>
#include <large_flock/common.hpp>
#include <large_flock/core/functions/scalar.hpp>
#include <large_flock/core/parser/scalar.hpp>
#include <large_flock/core/utils/template.hpp>

namespace large_flock
{

namespace core
{

nlohmann::json CoreScalarParsers::LlmMapScalarParser(DataChunk& args) {
	// Ensure that args contain key-value pairs
	if (args.ColumnCount() % 2 != 0) {
		throw std::runtime_error("LlmMapParser: args must contain key-value pairs.\nPossible keys are: template, model, max_tokens, temperature, <input name>");
	}

	nlohmann::json keys;
	std::unordered_set<std::string> required_keys = { "template" };
	std::unordered_map<std::string, int> optional_keys = {
		{"model", -1}, {"max_tokens", -1}, {"temperature", -1}
	};

	// Validate keys and populate the keys map
	for (idx_t i = 0; i < args.ColumnCount(); i += 2) {
		const auto& key_type = args.data[i].GetType();
		if (key_type != LogicalType::VARCHAR) {
			throw std::runtime_error("LlmMapParser: Key must be a string.");
		}
		std::string key = args.data[i].GetValue(0).ToString();
		keys[key] = i + 1; // Store index of the value
	}

	// Ensure that the template key is provided and is a string
	auto validate_key = [&](const std::string& key) {
		if (keys.find(key) == keys.end()) {
			if (optional_keys.find(key) != optional_keys.end()) {
				keys[key] = optional_keys[key];  // Assign default value for optional keys
			}
			else {
				throw std::runtime_error("LlmMapParser: you must pass a " + key + " to the llm_map function.");
			}
		}
		else if (key == "template" && args.data[keys[key]].GetType() != LogicalType::VARCHAR) {
			throw std::runtime_error("LlmMapParser: template must be a string.");
		}
		else if (key == "model" && args.data[keys[key]].GetType() != LogicalType::VARCHAR) {
			throw std::runtime_error("LlmMapParser: model must be a string.");
		}
		else if (key == "max_tokens" && args.data[keys[key]].GetType() != LogicalType::INTEGER) {
			throw std::runtime_error("LlmMapParser: max_tokens must be an integer.");
		}
		else if (key == "temperature" && args.data[keys[key]].GetType() != LogicalType::DECIMAL(2,1)) {
			throw std::runtime_error("LlmMapParser: temperature must be a decimal(2,1).");
		}
	};

	// Validate required and optional keys
	for (const auto& key : required_keys) {
		validate_key(key);
	}
	for (const auto& key : optional_keys) {
		validate_key(key.first);
	}

	// Ensure all placeholders in the template exist in the keys
	std::string template_str = args.data[keys["template"]].GetValue(0).ToString();
	auto placeholders = TemplateUtils::GetPlaceholders(template_str);
	for (const auto& placeholder : placeholders) {
		if (keys.find(placeholder) == keys.end()) {
			throw std::runtime_error("LlmMapParser: placeholder '" + placeholder + "' in the template is not found in the input keys.");
		}
	}

	// Populate the config JSON with key-value pairs from the keys map
	nlohmann::json config;
	for (const auto& el : keys.items()) {
		if (el.key() != "template" && el.key() != "model" && el.key() != "max_tokens" && el.key() != "temperature") {
			// Store additional parameters under 'inputs'
			config["inputs"][el.key()] = el.value();
		}
		else {
			config[el.key()] = el.value();
		}
	}

	return config;
}


} // namespace core

} // namespace large_flock