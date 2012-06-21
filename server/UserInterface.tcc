#ifndef USERINTERFACE_TCC_INCLUDED
#define USERINTERFACE_TCC_INCLUDED

#include "UserInterface.h"

namespace
{
	template <class T>
	struct printf_forwarder
	{
		static T forward(T const &t)
		{
			return t;
		}
	};

	template <class T>
	struct string_printf_forwarder
	{
		static typename T::value_type const *forward(T const &string)
		{
			return string.c_str();
		}
	};

	template <>
	struct printf_forwarder<std::string const &>
		: string_printf_forwarder<std::string>
	{
	};

	template <>
	struct printf_forwarder<std::string &>
		: string_printf_forwarder<std::string>
	{
	};

	template <>
	struct printf_forwarder<std::string>
		: string_printf_forwarder<std::string>
	{
	};

	template <>
	struct printf_forwarder<std::wstring const &>
		: string_printf_forwarder<std::wstring>
	{
	};

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
