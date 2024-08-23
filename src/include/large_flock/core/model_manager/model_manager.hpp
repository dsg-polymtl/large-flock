#pragma once
#include "large_flock/common.hpp"

namespace large_flock
{

namespace core
{

struct ModelManager
{
	static std::string OpenAI(const std::string& prompt, const std::string& model, const int& max_tokens, const float& temperature);
};

} // namespace core

} // namespace large_flock
