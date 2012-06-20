#ifndef DATABASE_TCC_INCLUDED
#define DATABASE_TCC_INCLUDED

#include "Database.h"

namespace
{
	template <class T>
	T sql_forward(T &&t);

	template <class T>
	struct sql_forwarder
	{
		static T forward(T &&t)
		{
			return t;
		}
	};

	template <>
	struct sql_forwarder<std::string>
	{
		static char const *forward(std::string &&string)
		{
			return string.c_str();
		}
	};
}

template <class... T>
Database::results_t Database::execute_sql(std::string const &statement, T &&... args)
{
	return execute_sqlv(statement.c_str(), sql_forward(args)...);
}

namespace
{
	template <class T>
	T sql_forward(T &&t)
	{
		return sql_forwarder<T>::forward(t);
	}
}

#endif
