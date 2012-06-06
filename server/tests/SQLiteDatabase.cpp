#include "SQLiteDatabase.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(SQLiteDatabaseSuite)

namespace
{
	/** a predicate for the boost test suite that always returns true
	 *  if a std::exception is passed
	 */
	bool stub_predicate(std::exception const &)
	{
		return true;
	}

	// some random messages with whitespaces/empty etc.
	std::vector<std::string> const g_sqlite_database_failure_messages {
		"foo",
		"\"  askd 20202020 NULL",
		"",
		"\n\t\v\b"
	};
}

BOOST_AUTO_TEST_CASE(construction)
{
	using database_errors::SQLiteConnectionError;
	using database_errors::SQLiteError;

	// construct with random strings
	for (auto const &message: g_sqlite_database_failure_messages)
	{
		BOOST_CHECK_NO_THROW(SQLiteConnectionError tmp(message));
		BOOST_CHECK_NO_THROW(SQLiteError tmp(message));
	}

	// check valid creation
	BOOST_CHECK_NO_THROW(SQLiteDatabase::from_path("./db.sql"));
	BOOST_CHECK_NO_THROW(SQLiteDatabase::temporary());

	// this should fail because / is not a file
	BOOST_CHECK_EXCEPTION(
		SQLiteDatabase::from_path("/"), SQLiteConnectionError, stub_predicate);

	/* this is supported by SQLite but not but our implementation, make sure
	 * it isn't
	 */
	BOOST_CHECK_EXCEPTION(
		SQLiteDatabase::from_path("file://uri"), std::runtime_error,
		stub_predicate);

	// those should fail because they don't start with / or ./
	BOOST_CHECK_EXCEPTION(
		SQLiteDatabase::from_path(".db.sql"), std::runtime_error,
		stub_predicate);
	BOOST_CHECK_EXCEPTION(
		SQLiteDatabase::from_path("invalid"), std::runtime_error,
		stub_predicate);
}

BOOST_AUTO_TEST_CASE(sql_complete)
{
	SQLiteDatabase sqlite_db = SQLiteDatabase::temporary();

	// complete sql statements end with ;
	BOOST_CHECK(!sqlite_db.complete_sql("CREATE TABLE foo"));
	BOOST_CHECK(sqlite_db.complete_sql("CREATE TABLE foo;"));
}

BOOST_AUTO_TEST_CASE(sql_execute)
{
	using database_errors::SQLiteError;

	/* check CREATE syntax according to some examples in
	 * http://www.sqlite.org/lang_createtable.html
	 */
	BOOST_CHECK_NO_THROW(
		SQLiteDatabase::temporary().
			execute_sql("CREATE TABLE foo (name);"));
	BOOST_CHECK_NO_THROW(
		SQLiteDatabase::temporary().
			execute_sql("CREATE TABLE foo (name INTEGER);"));
	BOOST_CHECK_EXCEPTION(
		SQLiteDatabase::temporary().
			execute_sql("CREATE TABLE foo;"), SQLiteError, stub_predicate);

}

BOOST_AUTO_TEST_CASE(exception_string_passing)
{
	using database_errors::SQLiteConnectionError;
	using database_errors::SQLiteError;

	// check if the string passed equals the one in what()
	for (auto const &message: g_sqlite_database_failure_messages)
	{
		BOOST_CHECK_EQUAL(message, SQLiteConnectionError(message).what());
		BOOST_CHECK_EQUAL(message, SQLiteError(message).what());
	}
}

BOOST_AUTO_TEST_SUITE_END()
