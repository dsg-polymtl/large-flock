#pragma once

#include <string>
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <dependencies/json.hpp> // Include the JSON library
#include <duckdb.hpp>

namespace duckdb
{
    // Function to handle the response data from the server
    size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

    // Function to perform the POST request to the LLM API
    std::string litellm(const std::string &prompt, const std::string &model = "gpt-3.5-turbo-instruct");
}