#pragma once
#include "large_flock/common.hpp"
#include "nlohmann/json.hpp"

namespace large_flock {

namespace core {

struct ModelManager {
    static nlohmann::json CallComplete(const std::string &prompt, const std::string &model,
                                       const nlohmann::json &settings, const bool json_response = true);
    static nlohmann::json CallEmbedding(const std::string &input, const std::string &model);
};

} // namespace core

} // namespace large_flock
