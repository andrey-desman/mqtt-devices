#pragma once

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include <vector>

template<typename T>
inline void noexception(const T& f)
{
	try
	{
		f();
	}
	catch (...)
	{
	}
}

template<typename E>
inline void check(bool condition, const char* message)
{
	if (!condition)
		throw E(message);
}

