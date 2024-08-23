#pragma once
#include "large_flock/common.hpp"

namespace large_flock
{

namespace core
{

struct CoreScalarFunctions
{
	static void Register(DatabaseInstance &db)
	{
		RegisterLlmMapScalarFunction(db);
	}

private:
	static void RegisterLlmMapScalarFunction(DatabaseInstance& db);
};

} // namespace core

} // namespace large_flock
