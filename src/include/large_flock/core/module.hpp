#pragma once
#include "large_flock/common.hpp"

namespace large_flock {

namespace core {

struct CoreModule {
public:
	static void Register(DatabaseInstance &db);
};

} // namespace core

} // namespace large_flock
