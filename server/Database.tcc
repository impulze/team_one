#ifndef DATABASE_TCC_INCLUDED
#define DATABASE_TCC_INCLUDED

#include "Database.h"

namespace
{
	/**
	 * Provide a construct which allows passing C++ types (also
	 * complex types that are not plain data). Those are then
	 * converted to plain old data types which can be handled
	 * by the C SQL API.
	 */
	template <class T>
	struct sql_forwarder
	{
		/**
		 * Forward (and possibly convert) a single C++ type
		 * to C.
		 *
		 * @param t The C++ variable.
		 * @return The converted C type.
		 */
		static T forward(T const &t)
		{
			return t;
		}
	};

	/**
	 * A simple converter for C++ strings to char pointers.
	 */
	template <class T>
	struct string_sql_forwarder
	{
		/**
		 * Convert a C++ string to a constant char pointer.
		 *
		 * @param string The C++ string variable.
		 * @return The converted char pointer (calls string.c_str()).
		 */
		static typename T::value_type const *forward(T const &string)
		{
			return string.c_str();
		}
	};

	/**
	 * A simple converter for C++ strings to char pointers.
	 * Specific for constant std::string references.
	 */
	template <>
	struct sql_forwarder<std::string const &>
		: string_sql_forwarder<std::string>
	{
	};

	/**
	 * A simple converter for C++ strings to char pointers.
	 * Specific for std::string references
	 */
	template <>
	struct sql_forwarder<std::string &>
		: string_sql_forwarder<std::string>
	{
	};

	/**
	 * A simple converter for C++ strings to char pointers.
	 * Specific for constant std::string rvalues.
	 */
	template <>
	struct sql_forwarder<std::string>
		: string_sql_forwarder<std::string>
	{
	};

	/**
	 * A simple converter for C++ strings to char pointers.
	 * Specific for constant std::wstring references.
	 */
	template <>
	struct sql_forwarder<std::wstring const &>
		: string_sql_forwarder<std::wstring>
	{
	};

	/**
	 * A simple converter for C++ strings to char pointers.
	 * Specific for std::wstring rvalues.
	 */
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
