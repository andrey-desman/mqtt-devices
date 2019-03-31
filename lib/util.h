#pragma once

#include <system_error>
#include <stdexcept>
#include <memory>
// #include <string_view>

#include <unistd.h>

struct fd_deleter
{
	void operator()(int* fd) { close(*fd); }
};

typedef std::unique_ptr<int, fd_deleter> fd_guard;

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

inline void check(int r, const char* message)
{
	if (r == -1)
		throw std::system_error(errno, std::system_category(), message);
}

inline void check(void* p, const char* message)
{
	if (!p)
		throw std::runtime_error(message);
}

#if 0
template<typename T>
std::string_view split(std::string_view& str, T delimiter)
{
	std::string_view::size_type pos = str.find_first_not_of(delimiter);
	if (pos == std::string_view::npos)
		return str = std::string_view();
	str.remove_prefix(pos);
	std::string_view res = str.substr(0, str.find_first_of(delimiter));
	str.remove_prefix(res.length());
	return res;
}
#endif
