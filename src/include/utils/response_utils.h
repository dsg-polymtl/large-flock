#pragma once

#include <string>
#include <vector>
#include <regex>
#include <duckdb.hpp>

#include "validation_utils.h"
#include "template_utils.h"
#include "litellm.h"
#include <duckdb.hpp>

namespace duckdb
{
    using KeyValueMap = std::unordered_map<std::string, std::unique_ptr<duckdb::Vector>>;

    std::vector<std::string> ParseApiResponse(const std::string &response_text, size_t num_responses);
    std::vector<std::string> SendRequestAndHandleResponse(const std::string &combined_prompt, size_t num_responses);
    std::vector<std::string> ChunkAndSendRequests(KeyValueMap &data_map, size_t size);
}

