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
	 * Move a SQLite database connection.
	 *
	 * The resources are moved and you can no longer expect
	 * any useful state of this object after moving it.
	 */
	SQLiteDatabase(SQLiteDatabase &&);

	/**
	 * Construct a new SQLite database connection.
	 *
	 * The path should be accessible. If the file does not exists, it is
	 * created.
	 *
	 * Valid paths either begin with '/' or './'.
	 *
	 * @param path The absolute or relative path to the database file.
	 * @throws std::runtime_error Thrown if the path is invalid.
	 * @throws database_errors::SQLiteConnectionError Thrown if an error occurs during
	 *                                                establishing the connection.
	 */
	static SQLiteDatabase from_path(std::string const &path);

	/**
	 * Construct a new SQLite database connection.
	 *
	 * The database is held in memory and automatically removed once the destructor
	 * is called.
	 *
	 * @throws database_errors::SQLiteConnectionError Thrown if an error occurs during
	 *                                                establishing the connection.
	 */
	static SQLiteDatabase temporary();

	/**
	 * Deconstruct a SQLite database object. Close the connection to the SQLite file.
	 */
	~SQLiteDatabase();

	bool complete_sql(std::string const &statement) const;

	/**
	 * Execute one SQL query.
	 *
	 * @param statement The statement to execute. The allowed and correct syntax can
	 *                  be found here: http://www.sqlite.org/lang.html
	 * @throws database_errors::Failure Thrown if any failure occurs during execution.
	 * @return The results of the passed SQL query.
	 */
	results_t execute_sql(std::string const &statement);

private:
	/**
	 * Construct a new SQLite database connection.
	 *
	 * This is basically a wrapper around the C API call and should
	 * be used by the public constructors only.
	 *
	 * @throws database_errors::SQLiteConnectionError Thrown if an error occurs during
	 *                                                establishing the connection.
	 */
	explicit SQLiteDatabase(std::string const &path);

private:
	// A handle to the SQLite3 database used in the C API.
	::sqlite3 *handle_;
};

#endif
