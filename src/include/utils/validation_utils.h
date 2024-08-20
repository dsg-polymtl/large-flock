#pragma once

#include <duckdb.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <iterator>
#include <duckdb.hpp>
#include "llm_extension.hpp"

namespace duckdb
{

    using KeyValueMap = std::unordered_map<std::string, std::unique_ptr<duckdb::Vector>>;

    // Validates the inputs to ensure that the arguments meet the requirements
    void InputsValidator(DataChunk &args);

    // Counts the number of words in a given string
    size_t CountWords(const std::string &str);

    // Estimates the number of tokens required for a given LogicalType
    size_t EstimateTokensForInput(LogicalType type);

    // Calculates the total number of tokens needed for the input arguments
    size_t CalculateTotalTokens(KeyValueMap &data_map);

    // Determines the chunk size based on the input arguments and a context size
    size_t DetermineChunkSize(KeyValueMap &data_map);

    KeyValueMap ParseKeyValuePairs(DataChunk &args);

} // namespace duckdb
