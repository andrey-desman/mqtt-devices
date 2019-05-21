#pragma once

#include <system_error>
#include <stdexcept>
#include <memory>
#include <string_view>
#include <optional>
#include <charconv>
#include <cassert>

#include <unistd.h>

struct fd_deleter
{
	void operator()(int* fd) noexcept { close(*fd); }
};

typedef std::unique_ptr<int, fd_deleter> fd_guard;

template<typename T>
inline void noexception(const T& f) noexcept
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

template<typename T>
std::string_view shift(std::string_view& str, T delimiter) noexcept
{
	std::string_view::size_type pos = str.find_first_not_of(delimiter);
	if (pos == std::string_view::npos)
		return str = std::string_view();
	str.remove_prefix(pos);
	std::string_view res = str.substr(0, str.find_first_of(delimiter));
	str.remove_prefix(res.length());
	return res;
}

template<typename T>
std::string_view pop(std::string_view& str, T delimiter) noexcept
{
	std::string_view::size_type pos = str.find_last_not_of(delimiter);
	if (pos == std::string_view::npos)
		return str = std::string_view();
	str.remove_suffix(str.length() - pos - 1);
	std::string_view res = str.substr(str.find_last_of(delimiter) + 1);
	str.remove_suffix(res.length());
	return res;
}

template<typename T>
std::optional<T> str_to_int(std::string_view str) noexcept
{
	T value;
	char const* end = str.data() + str.length();
	auto r = std::from_chars(str.data(), end, value);

	return r.ec == std::errc() && r.ptr == end ? std::make_optional(value) : std::nullopt;
}

template<typename T>
T cast_to_int(std::string_view str)
{
	using std::literals::operator""sv;
	if (auto r = str_to_int<T>(str))
		return *r;
	std::string w;
	w.append(1, '\'').append(str).append(1, '\'').append(" is not a valid integer"sv);
	throw std::runtime_error(w);
}

