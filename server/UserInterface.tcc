#ifndef USERINTERFACE_TCC_INCLUDED
#define USERINTERFACE_TCC_INCLUDED

#include "UserInterface.h"

namespace
{
	/**
	 * Provide a construct which allows passing C++ types (also
	 * complex types that are not plain data). Those are then
	 * converted to plain old data types which can be handled
	 * by the C printf API.
	 */
	template <class T>
	struct printf_forwarder
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
	struct string_printf_forwarder
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
	struct printf_forwarder<std::string const &>
		: string_printf_forwarder<std::string>
	{
	};

	/**
	 * A simple converter for C++ strings to char pointers.
	 * Specific for std::string references.
	 */
	template <>
	struct printf_forwarder<std::string &>
		: string_printf_forwarder<std::string>
	{
	};

	/**
	 * A simple converter for C++ strings to char pointers.
	 * Specific for std::string rvalues.
	 */
	template <>
	struct printf_forwarder<std::string>
		: string_printf_forwarder<std::string>
	{
	};

	/**
	 * A simple converter for C++ strings to char pointers.
	 * Specific for constant std::wstring references.
	 */
	template <>
	struct printf_forwarder<std::wstring const &>
		: string_printf_forwarder<std::wstring>
	{
	};

	/**
	 * A simple converter for C++ strings to char pointers.
	 * Specific for std::wstring rvalues.
	 */
	template <>
	struct printf_forwarder<std::wstring>
		: string_printf_forwarder<std::wstring>
	{
	};
}

template <class... T>
void UserInterface::printf(std::string const &format, T &&... args)
{
	return printfv(format.c_str(), printf_forwarder<T>::forward(args)...);
}

template <class Implementation>
Implementation &UserInterfaceSingleton<Implementation>::get_instance()
{
	if (!instance_)
	{
		instance_.reset(new Implementation());
	}

	return *instance_;
}

template <class T>
std::unique_ptr<T> UserInterfaceSingleton<T>::instance_;

#endif
