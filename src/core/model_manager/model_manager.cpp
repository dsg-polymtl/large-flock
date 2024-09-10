#include <large_flock/common.hpp>
#include <large_flock/core/model_manager/model_manager.hpp>
#include <large_flock/core/model_manager/openai.hpp>
#include <large_flock_extension.hpp>

namespace large_flock {
namespace core {

std::string ModelManager::CallComplete(const std::string &prompt, const std::string &model,
                                       const nlohmann::json &settings) {
    // List of supported models
    static const std::unordered_set<std::string> supported_models = {"gpt-4o", "gpt-4o-mini", "gpt-4-turbo", "gpt-4",
                                                                     "gpt-3.5-turbo"};

    // Check if the provided model is in the list of supported models
    if (supported_models.find(model) == supported_models.end()) {
        throw std::invalid_argument("Model '" + model +
                                    "' is not supported. Please choose one from the supported list: "
                                    "gpt-4o, gpt-4o-mini, gpt-4-turbo, gpt-4, gpt-3.5-turbo.");
    }

    // Get API key from the environment variable
    const char *key = std::getenv("OPENAI_API_KEY");
    if (!key) {
        throw std::runtime_error("OPENAI_API_KEY environment variable is not set.");
    } else {
        openai::start(); // Assume it uses the environment variable if no API
                         // key is provided
    }

    // check if settings is not empty and has max_tokens and temperature else make some default values
    auto max_tokens = 100;
    auto temperature = 0.5;
    if (!settings.empty()) {
        for (auto &[key, value] : settings.items()) {
            if (key == "max_tokens") {
                max_tokens = std::stoi(static_cast<std::string>(value));
            } else if (key == "temperature") {
                temperature = std::stof(static_cast<std::string>(value));
            } else {
                throw std::invalid_argument("Invalid setting key: " + key);
            }
        }
    }

    // Create a JSON request payload with the provided parameters
    nlohmann::json request_payload = {{"model", model},
                                      {"messages", {{{"role", "user"}, {"content", prompt}}}},
                                      {"max_tokens", max_tokens},
                                      {"temperature", temperature}};

    // Make a request to the OpenAI API
    auto completion = openai::chat().create(request_payload);

    // Extract the text from the response
    std::string response_text = completion["choices"][0]["message"]["content"].get<std::string>();

    // Trim leading newlines
    response_text.erase(0, response_text.find_first_not_of("\n"));

    return response_text;
}

} // namespace core
} // namespace large_flock