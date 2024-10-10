#pragma once
#include "large_flock/common.hpp"

#include <nlohmann/json.hpp>

namespace large_flock {

namespace core {

struct CoreScalarParsers {
    static std::vector<nlohmann::json> Struct2Json(Vector &struct_vector, int size);
    static void LfMapScalarParser(DataChunk &args);
    static void LfGenerateScalarParser(DataChunk &args);
    static void LfEmbeddingScalarParser(DataChunk &args);
};

} // namespace core

} // namespace large_flock
