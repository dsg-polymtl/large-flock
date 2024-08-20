#pragma once

#include <string>
#include <vector>
#include <inja/inja.hpp>
#include <sstream>

#include <duckdb.hpp>

namespace duckdb
{
    using KeyValueMap = std::unordered_map<std::string, std::unique_ptr<duckdb::Vector>>;
    
    std::string GenerateCombinedPrompt(const inja::json &context);
    inja::json CreatePromptContext(KeyValueMap &data_map, size_t start_index, size_t end_index);
}
