#ifndef DATABASE_H_INCLUDED
#define DATABASE_H_INCLUDED

#include <unordered_map>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @file Database.h
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Database interface and common objects required for accessing
 * databases.
 */

namespace database_errors
{
	/**
	 * A generic database exception.
	 * Specific errors derive from this class to create
	 * an exception hierarchy.
	 */
	struct Failure
		: std::runtime_error
	{
		/**
		 * Construct a new database failure with a specific
		 * error message.
		 *
		 * @param message The error message that describes the error.
		 */
		Failure(std::string const &message);
	};
}

/**
 * An interface for the database portion of the server.
 * It offers member functions for execution of SQL queries.
 */
class Database
{
public:
	/**
	 * Construct a database object.
	 * Provide a default constructor, because this is just an interface.
	 */
	Database() = default;

	/**
	 * Deconstruct a database object, freeing all its resources and closing
	 * the connection.
	 */
	virtual ~Database();

	/**
	 * Delete the default copy constructor, making copying a database
	 * object impossible.
	 */
	Database(Database const &) = delete;

	/**
	 * Delete the default assignment operator, making assigning a database
	 * object impossible.
	 */
	Database &operator=(Database const &) = delete;

	// A result consisting of column name and value.
	typedef std::unordered_map<std::string, std::string> result_t;

	// A list of results.
	typedef std::vector<result_t> results_t;

	/**
	 * Check if the SQL statement is complete.
	 *
	 * @param statement The statement to check.
	 * @return 'true' if the statement can be processed by the underlying
	 *         implementation, 'false' otherwise.
	 */
	virtual bool complete_sql(std::string const &statement) const = 0;

	/**
	 * Execute one SQL query.
	 *
	 * Use %q in the format for placeholders.
	 *
	 * @param statement The statement (including placeholders) to execute.
	 * @param args The values for the placeholders.
	 * @throws database_errors::Failure Thrown if any failure occurs during execution.
	 * @return The results of the passed SQL query.
	 */
	template <class... T>
	results_t execute_sql(std::string const &statement, T &&... args);

private:
	/**
	 * Execute one SQL query.
	 *
	 * Use %q in the format for placeholders.
	 *
	 * @param statement The statement (including placeholders) to execute.
	 * @param ... Variable arguments in plain old data.
	 * @throws database_errors::Failure Thrown if any failure occurs during execution.
	 * @return The results of the passed SQL query.
	 */
	virtual results_t execute_sqlv(char const *statement, ...) = 0;
};

#include "Database.tcc"

#endif
