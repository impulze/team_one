#include "SQLiteDatabase.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(SQLiteDatabaseSuite)

namespace
{
	void ctor_wrong_database()
	{
		SQLiteDatabase("/");
	}

	void ctor_good_database()
	{
		SQLiteDatabase("db.sql");
	}

	bool stub_predicate(std::exception const &)
	{
		return true;
	}
}

using namespace database_errors;

BOOST_AUTO_TEST_CASE(construction)
{
	BOOST_CHECK_EXCEPTION(ctor_wrong_database(), SQLiteConnectionError, stub_predicate);
	BOOST_CHECK_NO_THROW(ctor_good_database());
}

BOOST_AUTO_TEST_CASE(sql_complete)
{
	SQLiteDatabase sqlite_db("db.sql");

	BOOST_CHECK(!sqlite_db.complete_sql("CREATE TABLE foo"));
	BOOST_CHECK(sqlite_db.complete_sql("CREATE TABLE foo;"));
}

BOOST_AUTO_TEST_SUITE_END()
