#ifndef SQLITEDATABASE_H_INCLUDED
#define SQLITEDATABASE_H_INCLUDED

#include "Database.h"

// Forward declaration of the SQLite3 handle type.
struct sqlite3;

namespace database_errors
{
	/**
	 * A generic database exception of the SQLite implementation.
	 * Specific errors derive from this class to create
	 * an exception hierarchy.
	 */
	struct SQLiteError
		: Failure
	{
		/**
		 * Construct a new SQLite database failure with a specific
		 * error message.
		 *
		 * @param message The error message that describes the error.
		 */
		SQLiteError(std::string const &message);
	};

	/**
	 * A database connection exception of the SQLite implementation.
	 */
	struct SQLiteConnectionError
		: SQLiteError
	{
		/**
		 * Construct a new SQLite database failure describing
		 * an error while handling the connection.
		 *
		 * @param message The error message that describes the error.
		 */
		SQLiteConnectionError(std::string const &message);
	};
}

/**
 * The SQLite implementation of the database interface.
 */
class SQLiteDatabase
	: public Database
{
public:
	/**
	 * Construct a new SQLite database connection.
	 *
	 * @param path The absolute or relative path to the database file.
	 * @throws database_errors::SQLiteConnectionError Thrown if an error occurs during
	 *                                                establishing the connection.
	 */
	SQLiteDatabase(std::string const &path);

	/**
	 * Deconstruct a SQLite database object. Close the connection to the SQLite file.
	 */
	~SQLiteDatabase();

	bool correct_sql(std::string const &statement) const;

	results_t execute_sql(std::string const &statement);

private:
	// A handle to the SQLite3 database used in the C API.
	::sqlite3 *handle_;
};

#endif
