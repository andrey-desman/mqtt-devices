#pragma once

#include <syslog.h>

#include <cstdio>
#include <cstdarg>

namespace logger
{
	enum level
	{
		error,
		warning,
		info,
		debug,
	};

	constexpr const char* file_name_iterate(const char* str, const char* last)
	{
		return *str ?
			(*str == '/' ? file_name_iterate(str + 1, str + 1) : file_name_iterate(str + 1, last)) :
			last;
	}

	constexpr const char* file_name(const char* str)
	{
		return file_name_iterate(str, str);
	}

	inline const char* level_to_str(level l)
	{
		switch (l)
		{
			case error: return "ERR";
			case warning: return "WRN";
			case info: return "INF";
			case debug: return "DBG";
		}
		return "NIL";
	}

	inline int level_to_syslog(level l)
	{
		switch(l)
		{
			case error: return LOG_ERR;
			case warning: return LOG_WARNING;
			case info: return LOG_NOTICE;
			case debug: return LOG_DEBUG;
		}
		return LOG_INFO;
	}

	inline void out(level l, const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		if (true)
			vsyslog(level_to_syslog(l), format, args);
		else
			vprintf(format, args);
		va_end(args);
	}

	extern const char* APP_NAME;
	extern const char* INSTANCE_NAME;

	inline void init(const char* app_name, const char* instance_name)
	{
		APP_NAME = app_name;
		INSTANCE_NAME = instance_name;
		if (false)
			openlog(INSTANCE_NAME, LOG_PERROR, LOG_USER);
	}

} // namespace

#define FILE_NAME ({const char* fn = logger::file_name(__FILE__); fn;})

#define LOG(l, format, ...) logger::out(logger::l, "[%s] [%s/%s] %s:%d: %s: " format "\n", logger::level_to_str(logger::l), logger::APP_NAME, logger::INSTANCE_NAME, FILE_NAME, __LINE__, __func__, ##__VA_ARGS__)
#define LOGL(l, fn, line, func, format, ...) logger::out(logger::l, "[%s] [%s/%s] %s:%d: %s: " format "\n", logger::level_to_str(logger::l), logger::APP_NAME, logger::INSTANCE_NAME, fn, line, func, ##__VA_ARGS__)

