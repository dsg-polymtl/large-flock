#pragma once

#include <string>
#include <vector>
#include <inja/inja.hpp>
#include <sstream>

#include <duckdb.hpp>

namespace duckdb
{
    std::string replace_placeholders(const std::string &template_str, const std::vector<std::string> &args);
    std::string GenerateCombinedPrompt(const inja::json &context);
    inja::json CreatePromptContext(duckdb::DataChunk &args, size_t start_index, size_t end_index, const std::vector<std::reference_wrapper<duckdb::Vector>> &args_vector);
}
