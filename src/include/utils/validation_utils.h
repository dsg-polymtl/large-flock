#pragma once

#include <duckdb.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <iterator>
#include <duckdb.hpp>

namespace duckdb
{
    void InputsValidator(DataChunk &args);
    size_t CountWords(const std::string &str);
    size_t EstimateTokensForInput(LogicalType type);
    size_t CalculateTotalTokens(DataChunk &args);
    size_t DetermineChunkSize(DataChunk &args);
}

