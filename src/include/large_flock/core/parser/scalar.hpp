#pragma once
#include "large_flock/common.hpp"

#include <nlohmann/json.hpp>

namespace large_flock {

namespace core {

struct CoreScalarParsers {
    static nlohmann::json LfMapScalarParser(DataChunk &args);
};

} // namespace core

} // namespace large_flock
