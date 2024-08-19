#include "utils/openai_utils.h"

// Function to get response from OpenAI
std::string GetOpenAIResponse(const std::string &prompt)
{
    // Start OpenAI with the environment variable API key
    openai::start();

    // Create a JSON request payload
    nlohmann::json request_payload = {
        {"model", "gpt-3.5-turbo-instruct"},
        {"prompt", prompt},
        {"max_tokens", 100},
        {"temperature", 0.5}};

    // Make a request to the OpenAI API
    auto completion = openai::completion().create(request_payload);

    // Extract the text from the response
    std::string response_text = completion["choices"][0]["text"].get<std::string>();

    // Trim leading newlines
    response_text.erase(0, response_text.find_first_not_of("\n"));

    return response_text;
}
