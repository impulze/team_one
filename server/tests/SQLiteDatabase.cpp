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
}

BOOST_AUTO_TEST_CASE(construction)
{
	using database_errors::SQLiteConnectionError;

	BOOST_CHECK_NO_THROW(SQLiteDatabase::from_path("./db.sql"));
	BOOST_CHECK_NO_THROW(SQLiteDatabase::temporary());
	BOOST_CHECK_EXCEPTION(
		SQLiteDatabase::from_path("/"), SQLiteConnectionError, stub_predicate);
	BOOST_CHECK_EXCEPTION(
		SQLiteDatabase::from_path("file://uri"), std::runtime_error,
		stub_predicate);
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

	BOOST_CHECK(!sqlite_db.complete_sql("CREATE TABLE foo"));
	BOOST_CHECK(sqlite_db.complete_sql("CREATE TABLE foo;"));
}

BOOST_AUTO_TEST_CASE(sql_execute)
{
	using database_errors::SQLiteError;

	SQLiteDatabase sqlite_db = SQLiteDatabase::temporary();

	BOOST_CHECK_NO_THROW(sqlite_db.execute_sql("CREATE TABLE foo (INTEGER);"));
	BOOST_CHECK_EXCEPTION(
		sqlite_db.execute_sql("CREATE TABLE foo;"), SQLiteError, stub_predicate);

}

BOOST_AUTO_TEST_SUITE_END()
