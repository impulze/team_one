#ifndef DATABASE_TCC_INCLUDED
#define DATABASE_TCC_INCLUDED

#include "Database.h"

template <class... T>
Database::results_t Database::execute_sql(std::string const &statement, T &&... args)
{
	return execute_sqlv(statement.c_str(), std::forward<T>(args)...);
}

#endif
