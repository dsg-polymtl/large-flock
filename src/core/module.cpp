#include "large_flock/core/module.hpp"

#include "large_flock/common.hpp"
#include "large_flock/core/config/lf_config.hpp"
#include "large_flock/core/functions/scalar.hpp"

namespace large_flock {
namespace core {

void CoreModule::Register(DatabaseInstance &db) {
    CoreScalarFunctions::Register(db);
    LfConfig::Configure(db);
}

} // namespace core
} // namespace large_flock
