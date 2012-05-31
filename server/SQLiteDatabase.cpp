#include "SQLiteDatabase.h"

#include <cassert>
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

namespace
{
	int execute_sql_callback(void *results, int columns,
	                         char **column_results,
	                         char **column_names);
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

bool SQLiteDatabase::complete_sql(std::string const &statement) const
{
	return sqlite3_complete(statement.c_str()) != 0;
}

SQLiteDatabase::results_t SQLiteDatabase::execute_sql(std::string const &statement)
{
	using database_errors::SQLiteError;

	results_t results;
	char *sqlite_error_string;
	int const result = sqlite3_exec(
		handle_, statement.c_str(),
		execute_sql_callback, &results,
		&sqlite_error_string);

	/**
	 * SQLiteFree is a class which only exists to free the resources
	 * allocated by sqlite3 in this scope.
	 * Even if this implementation throws an error the constructor
	 * of this class is called and the resources properly freed.
	 */
	struct SQLiteFree
	{
		SQLiteFree(char *error_string)
			: error_string(error_string)
		{
		}

		~SQLiteFree()
		{
			sqlite3_free(error_string);
		}

		char *error_string;
	};

	SQLiteFree sqlite_free_object(sqlite_error_string);

	if (result)
	{
		if (sqlite_error_string)
		{
			throw SQLiteError(sqlite_error_string);
		}
		else
		{
			throw SQLiteError("unknown SQLite error");
		}
	}

	return results;
}

namespace
{
	int execute_sql_callback(void *results_ptr, int columns,
	                         char **column_results,
	                         char **column_names)
	{
		Database::result_t result;

		for (int i = 0; i < columns; i++)
		{
			// every column should have a non-NULL name
			assert(column_names[i]);

			result[column_names[i]] = column_results[i];
		}

		Database::results_t &results =
			*static_cast<Database::results_t *>(results_ptr);

		results.push_back(result);

		return 0;
	}
}
