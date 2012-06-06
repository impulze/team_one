#include "Database.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(DatabaseSuite)

namespace
{
	// some random messages with whitespaces/empty etc.
	std::vector<std::string> const g_database_failure_messages {
		"foo",
		"\"  askd 20202020 NULL",
		"",
		"\n\t\v\b"
	};
}

BOOST_AUTO_TEST_CASE(construction)
{
	using database_errors::Failure;

	// construct with random strings
	for (auto const &message: g_database_failure_messages)
	{
		BOOST_CHECK_NO_THROW(Failure tmp(message));
	}
}

BOOST_AUTO_TEST_CASE(string_passing)
{
	using database_errors::Failure;

	// check if the string passed equals the one in what()
	for (auto const &message: g_database_failure_messages)
	{
		BOOST_CHECK_EQUAL(message, Failure(message).what());
	}
}

BOOST_AUTO_TEST_SUITE_END()
