#ifndef DATABASE_TCC_INCLUDED
#define DATABASE_TCC_INCLUDED

#include "Database.h"

namespace
{
	template <class T>
	struct sql_forwarder
	{
		static T forward(T const &t)
		{
			return t;
		}
	};

	template <class T>
	struct string_sql_forwarder
	{
		static typename T::value_type const *forward(T const &string)
		{
			return string.c_str();
		}
	};

	template <>
	struct sql_forwarder<std::string const &>
		: string_sql_forwarder<std::string>
	{
	};

	template <>
	struct sql_forwarder<std::string &>
		: string_sql_forwarder<std::string>
	{
	};

	template <>
	struct sql_forwarder<std::string>
		: string_sql_forwarder<std::string>
	{
	};

	template <>
	struct sql_forwarder<std::wstring const &>
		: string_sql_forwarder<std::wstring>
	{
	};

	template <>
	struct sql_forwarder<std::wstring>
		: string_sql_forwarder<std::wstring>
	{
	};
}

template <class... T>
Database::results_t Database::execute_sql(std::string const &statement, T &&... args)
{
	return execute_sqlv(statement.c_str(), sql_forwarder<T>::forward(args)...);
}

#endif
