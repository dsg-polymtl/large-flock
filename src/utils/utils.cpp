#include "utils.h"
#include <sstream>
#include <nlohmann/json.hpp>
#include <openai.hpp>
#include <regex>

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

std::vector<std::string> ParseApiResponse(const std::string &response_text, size_t num_responses)
{
    std::vector<std::string> responses;

    // Define the regex pattern to match "Response {i}: content"
    std::regex response_pattern(R"(Response (\d+):\s*(.*?)(?=\s*Response \d+:|$))");
    std::smatch match;

    auto it = response_text.cbegin();
    auto end = response_text.cend();

    // Iterate over all matches
    while (std::regex_search(it, end, match, response_pattern))
    {
        if (match.size() == 3)
        { // Match group 1 is the index, group 2 is the content
            std::string content = match[2].str();
            responses.push_back(content);
        }
        it = match.suffix().first; // Move iterator to the end of the current match
    }

    // Ensure we have exactly the number of responses expected
    if (responses.size() < num_responses)
    {
        responses.resize(num_responses, ""); // Fill missing responses with empty strings
    }

    return responses;
}