#pragma once

#include <string>
#include <vector>
#include <regex>
#include <duckdb.hpp>

#include "openai_utils.h"
#include "validation_utils.h"
#include "template_utils.h"
#include <duckdb.hpp>

namespace duckdb
{
    std::vector<std::string> ParseApiResponse(const std::string &response_text, size_t num_responses);
    std::vector<std::string> SendRequestAndHandleResponse(const std::string &combined_prompt, size_t num_responses);
    std::vector<std::string> ChunkAndSendRequests(DataChunk &args);
}

