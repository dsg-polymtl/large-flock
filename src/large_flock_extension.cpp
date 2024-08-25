#define DUCKDB_EXTENSION_MAIN

#include "large_flock_extension.hpp"

#include "large_flock/common.hpp"
#include "large_flock/core/module.hpp"

namespace duckdb {

static void LoadInternal(DatabaseInstance &instance) {
    large_flock::core::CoreModule::Register(instance);
}

void LargeFlockExtension::Load(DuckDB &db) {
    LoadInternal(*db.instance);
}

std::string LargeFlockExtension::Name() {
    return "large_flock";
}
std::string LargeFlockExtension::Version() const {
#ifdef EXT_VERSION_LARGE_FLOCK
    return EXT_VERSION_LARGE_FLOCK;
#else
    return "";
#endif
}

} // namespace duckdb

extern "C" {

DUCKDB_EXTENSION_API void large_flock_init(DatabaseInstance &db) {
    LoadInternal(db);
}

DUCKDB_EXTENSION_API const char *large_flock_version() {
    return duckdb::DuckDB::LibraryVersion();
}
}

#ifndef DUCKDB_EXTENSION_MAIN
#error DUCKDB_EXTENSION_MAIN not defined
#endif
