#pragma once

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include <vector>

struct converter
{
	const std::vector<std::string>& args;
	size_t offset;

	converter(const std::vector<std::string>& v, size_t offset)
	: args(v), offset(offset) { }

	template<typename T>
	T get_impl(T*)
	{
		return boost::lexical_cast<T>(args.at(offset));
	}

	template<typename T>
	boost::optional<T> get_impl(boost::optional<T>*)
	{
		try
		{
			return boost::lexical_cast<T>(args.at(offset));
		}
		catch (const std::out_of_range&)
		{
			return boost::none;
		}
	}

	template<typename T>
	operator T() { return get_impl(static_cast<T*>(0)); }
};

template<typename T>
void noexception(const T& f)
{
	try
	{
		f();
	}
	catch (...)
	{
	}
}

