#pragma once
#include "large_flock/common.hpp"

namespace large_flock {

namespace core {

struct CoreScalarFunctions {
    static void Register(DatabaseInstance &db) {
        RegisterLfMapScalarFunction(db);
        RegisterLfGenerateScalarFunction(db);
        RegisterLfEmbeddingScalarFunction(db);
    }

private:
    static void RegisterLfMapScalarFunction(DatabaseInstance &db);
    static void RegisterLfGenerateScalarFunction(DatabaseInstance &db);
    static void RegisterLfEmbeddingScalarFunction(DatabaseInstance &db);
};

} // namespace core

} // namespace large_flock
