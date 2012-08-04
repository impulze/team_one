#include "SQLiteDatabase.h"

#include <boost/test/unit_test.hpp>

/**
 * @file server/tests/SQLiteDatabase.cpp
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Unit tests for the SQLiteDatabase.
 */

//! create the SQLite database testsuite
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

//! test the construction
BOOST_AUTO_TEST_CASE(construction)
{
	using database_errors::SQLiteConnectionError;
	using database_errors::SQLiteError;

	// construct with random strings
	for (auto const &message: g_sqlite_database_failure_messages)
	{
		BOOST_CHECK_NO_THROW(SQLiteConnectionError tmp(message));
		BOOST_CHECK_NO_THROW(SQLiteError<> tmp(message));
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
	BOOST_CHECK_EXCEPTION(
		SQLiteDatabase::from_path("-"), std::runtime_error,
		stub_predicate);
}

//! test if the implementation can handle basic sql
BOOST_AUTO_TEST_CASE(sql_complete)
{
	SQLiteDatabase sqlite_db = SQLiteDatabase::temporary();

	// complete sql statements end with ;
	BOOST_CHECK(!sqlite_db.complete_sql("CREATE TABLE foo"));
	BOOST_CHECK(sqlite_db.complete_sql("CREATE TABLE foo;"));
}

//! test if the implementation can execute basic sql
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
			execute_sql("CREATE TABLE foo;"), SQLiteError<>, stub_predicate);

	// destructor shouldn't throw
	SQLiteDatabase sqlite_db = SQLiteDatabase::temporary();

	BOOST_CHECK_NO_THROW(sqlite_db.~SQLiteDatabase());
}

//! test if proper exceptions are passed
BOOST_AUTO_TEST_CASE(exception_string_passing)
{
	using database_errors::SQLiteConnectionError;
	using database_errors::SQLiteError;

	// check if the string passed equals the one in what()
	for (auto const &message: g_sqlite_database_failure_messages)
	{
		BOOST_CHECK_EQUAL("sqlite error: " + message, SQLiteConnectionError(message).what());
		BOOST_CHECK_EQUAL("sqlite error: " + message, SQLiteError<>(message).what());
	}
}

//! end the testsuite
BOOST_AUTO_TEST_SUITE_END()
