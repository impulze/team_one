#include "Database.h"

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
