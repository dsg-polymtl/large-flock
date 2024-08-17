#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

// Function to get response from OpenAI
std::string GetOpenAIResponse(const std::string &prompt);

// Function to replace placeholders in a template string
std::string replace_placeholders(const std::string &template_str, const std::vector<std::string> &args);

// Function to parse API response based on "Response {i}" format
std::vector<std::string> ParseApiResponse(const std::string &response_text, size_t num_responses);

#endif // UTILS_H
