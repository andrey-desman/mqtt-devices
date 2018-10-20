#pragma once

#include <system_error>
#include <stdexcept>
#include <memory>

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

