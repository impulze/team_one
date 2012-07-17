#include "SQLiteDatabase.h"

#include <cassert>
#include <cstdarg>
#include <iostream>
#include <stdexcept>

#include <sqlite3.h>

/**
 * @file server/SQLiteDatabase.cpp
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Implementation file for the SQLite3 implementation for the Database interface.
 */

namespace database_errors
{
	template <class Base>
	SQLiteError<Base>::SQLiteError(std::string const &message)
		: Base("sqlite error: " + message)
	{
	}

	SQLiteConnectionError::SQLiteConnectionError(std::string const &message)
		: SQLiteError(message)
	{
	}

	SQLiteConstraintError::SQLiteConstraintError(std::string const &message)
		: SQLiteError<database_errors::ConstraintError>(message)
	{
	}
}

namespace
{
	/**
	 * This function is used by the SQLite3 C API.
	 * It will be called by the internal API for adding
	 * results.
	 *
	 * @param results_ptr A pointer the the C++ structure for storing
	 *                    result pairs. This was passed to the underlying
	 *                    API when registering the callback.
	 * @param columns The amount of fetched columns.
	 * @param column_results An array of values as strings.
	 * @param column_names An array of names as strings.
	 *
	 * @return Always returns 0.
	 */
	int execute_sql_callback(void *results_ptr, int columns,
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
	if (!((path.length() >= 1 && path[0] == '/') ||
	      (path.length() >= 2 && path[0] == '.' && path[1] == '/')))
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

	// make sure that a second destruction results in a no-op
	handle_ = 0;
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

SQLiteDatabase::results_t SQLiteDatabase::execute_sqlv(char const *format, ...)
{
	using database_errors::SQLiteError;
	using database_errors::SQLiteConstraintError;

	va_list list;
	results_t results;
	char *sqlite_error_string;

	va_start(list, format);
	char *sqlite_statement = sqlite3_vmprintf(format, list);
	va_end(list);

	if (!sqlite_statement)
	{
		throw std::bad_alloc();
	}

	int const result = sqlite3_exec(
		handle_, sqlite_statement,
		execute_sql_callback, &results,
		&sqlite_error_string);

	sqlite3_free(sqlite_statement);

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
		std::string failure = "unknown SQLite error";

		if (sqlite_error_string)
		{
			failure = sqlite_error_string;
		}

		if (result == 19)
		{
			throw SQLiteConstraintError(failure);
		}

		throw SQLiteError<>(failure);
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
