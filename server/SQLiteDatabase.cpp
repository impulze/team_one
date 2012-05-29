#include "SQLiteDatabase.h"

#include <iostream>

#include <sqlite3.h>

namespace database_errors
{
	SQLiteError::SQLiteError(std::string const &message)
		: Failure(message)
	{
	}

	SQLiteConnectionError::SQLiteConnectionError(std::string const &message)
		: SQLiteError(message)
	{
	}
}

SQLiteDatabase::SQLiteDatabase(std::string const &path)
{
	int const result = sqlite3_open(path.c_str(), &handle_);

	if (result)
	{
		// throw an error if the connection failed
		char const *error = sqlite3_errmsg(handle_);

		throw database_errors::SQLiteConnectionError(error);
	}
}

SQLiteDatabase::~SQLiteDatabase()
{
	int const result = sqlite3_close(handle_);

	if (result)
	{
		// just print a warning, don't throw in the destructor
		char const *error = sqlite3_errmsg(handle_);

		std::cerr << error << '\n';
	}
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
bool SQLiteDatabase::correct_sql(std::string const &statement) const
{
	throw std::logic_error("unimplemented");
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
SQLiteDatabase::results_t SQLiteDatabase::execute_sql(std::string const &statement)
{
	throw std::logic_error("unimplemented");
}
#pragma GCC diagnostic pop
