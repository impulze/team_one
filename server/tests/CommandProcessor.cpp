#include "CommandProcessor.h"
#include "NCursesUserInterface.h"
#include "SQLiteDatabase.h"
#include "UserDatabase.h"

#include <cstdarg>
#include <cstdio>

#include <boost/test/unit_test.hpp>

/**
 * @file server/tests/CommandProcessor.cpp
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Unit tests for the CommandProcessor.
 */

//! create the command processor testsuite
BOOST_AUTO_TEST_SUITE(CommandProcessorSuite)

namespace
{
	class TestUserInterface
		: public UserInterface
	{
		void printfv(char const *format, ...)
		{
			va_list list;
			std::string new_format(format);

			new_format.insert(0, "TEST UI: ");

			va_start(list, format);
			std::vprintf(new_format.c_str(), list);
			va_end(list);
		}

		void run()
		{
		}
	};
}

//! simple typedef for easier access
typedef UserInterfaceSingleton<TestUserInterface> TestUserInterfaceSingleton;

//! test the construction
BOOST_AUTO_TEST_CASE(construction)
{
	auto &ui = TestUserInterfaceSingleton::get_instance();
	UserDatabase db(std::make_shared<SQLiteDatabase>(SQLiteDatabase::temporary()), ui);

	BOOST_CHECK_NO_THROW(CommandProcessor cp(ui, db));
}

//! end the testsuite
BOOST_AUTO_TEST_SUITE_END()
