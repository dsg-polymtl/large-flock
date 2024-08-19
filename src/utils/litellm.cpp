#include "utils/litellm.h"
#include "duckdb.hpp"

namespace duckdb
{
    // Function to handle the response data from the server
    size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
    {
        ((std::string *)userp)->append((char *)contents, size * nmemb);
        return size * nmemb;
    }

    std::string litellm(const std::string &prompt, const std::string &model)
    {
        CURL *curl;
        CURLcode res;
        std::string read_buffer;

        // Initialize libcurl
        curl = curl_easy_init();
        if (curl)
        {
            // Use nlohmann/json to create the JSON payload
            nlohmann::json json_payload = {
                {"model", model},
                {"messages", {{{"role", "user"}, {"content", prompt}}}}};

            std::string json_str = json_payload.dump(); // Convert JSON object to string

            // Set the URL
            curl_easy_setopt(curl, CURLOPT_URL, "http://0.0.0.0:4000/chat/completions");

            // Specify that we are sending a POST request
            curl_easy_setopt(curl, CURLOPT_POST, 1L);

            // Set the POST data
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str.c_str());

            // Set the Content-Type header
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // Set the write callback function to capture the response
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);

            // Perform the request
            res = curl_easy_perform(curl);

            // Clean up
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);

            // Check for errors
            if (res != CURLE_OK)
            {
                throw InvalidInputException("Response error from litellm server");
            }
            else
            {
                nlohmann::json response_json = nlohmann::json::parse(read_buffer);
                std::string content = response_json["choices"][0]["message"]["content"];

                return content;
            }
        }

        throw InvalidInputException("Curl can't be initialized");
    }
}