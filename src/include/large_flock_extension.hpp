#pragma once

#include "large_flock/common.hpp"

namespace duckdb {

class LargeFlockExtension : public Extension {
public:
    void Load(DuckDB &db) override;
    std::string Name() override;
    std::string Version() const override;
};

} // namespace duckdb
