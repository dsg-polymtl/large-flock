#pragma once

#include <string>
#include <dependencies/openai.hpp>
#include <duckdb.hpp>


std::string GetOpenAIResponse(const std::string &prompt);

