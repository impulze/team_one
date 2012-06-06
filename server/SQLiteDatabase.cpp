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

SQLiteDatabase::SQLiteDatabase(SQLiteDatabase &&other)
	: handle_(other.handle_)
{
	other.handle_ = 0;
}

SQLiteDatabase SQLiteDatabase::from_path(std::string const &path)
{
	if (path[0] != '/' && (path[0] != '.' || path[1] != '/'))
	{
		throw std::runtime_error("invalid path, should begin with either '/' or './'");
	}

	return SQLiteDatabase(path);
}

SQLiteDatabase SQLiteDatabase::temporary()
{
	return SQLiteDatabase(":memory:");
}

SQLiteDatabase::~SQLiteDatabase()
{
	int const result = sqlite3_close(handle_);

	/** according to the sqlite3 C API the close call should only
	 *  return non-zero if there are outstanding transactions and since
	 *  this implementation only offers a wrapper around full SQL statements
	 *  (commited and finalized), this should never be non-zero
	 */
	assert(result == 0);

#if 0
	if (result)
	{
		// just print a warning, don't throw in the destructor
		char const *error = sqlite3_errmsg(handle_);

		std::cerr << error << '\n';
	}
#endif
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

SQLiteDatabase::SQLiteDatabase(std::string const &path)
{
	int const result = sqlite3_open_v2(
		path.c_str(), &handle_,
		SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
		NULL);

	if (result)
	{
		// throw an error if the connection failed
		char const *error = sqlite3_errmsg(handle_);

		throw database_errors::SQLiteConnectionError(error);
	}
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

			// create a mapping from column name to result string
			result[column_names[i]] = column_results[i];
		}

		// obtain a reference to the passed output results
		Database::results_t &results =
			*static_cast<Database::results_t *>(results_ptr);

		results.push_back(result);

		return 0;
	}
}
