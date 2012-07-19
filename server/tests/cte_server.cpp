//! name of the master test suite module
#define BOOST_TEST_MODULE Master Test Suite
#include <boost/test/included/unit_test.hpp>

/**
 * @file server/tests/cte_server.cpp
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Stub file to get a working unit test binary.
 */

class UserInterface;

//! TODO: quite dirty way to share the user interface
UserInterface *g_user_interface;
