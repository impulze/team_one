#include "Database.h"

/**
 * @file Database.cpp
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Implementation file for common abstractions of Database things.
 */

namespace database_errors
{
	Failure::Failure(std::string const &message)
		: std::runtime_error(message)
	{
	}
}

Database::~Database()
{
}
