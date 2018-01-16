#pragma once

#include "log.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "selene.h"

#include <stdexcept>
#include <boost/lexical_cast.hpp>

class options
{
public:
	enum option_scope
	{
		scope_device,
		scope_domain,
		scope_global
	};

	options(const std::string& domain, int argc, char* argv[]);

	std::string get_name() const
	{
		return name_;
	}

	template<typename T>
	T get(const std::string& opt, option_scope scope)
	{
		sel::Selector s = L_[domain_.c_str()][name_.c_str()][opt.c_str()];

		if (!s && scope >= scope_domain)
			s = L_[domain_.c_str()][opt.c_str()];
		if (s)
		if (!s && scope >= scope_global)
			s = L_[opt.c_str()];
		if (!s)
		{
			LOG(info, "missing required option '%s'", opt.c_str());
			throw std::runtime_error("missing required option");
		}
		else
		{
			std::string val = s;
			LOG(info, "'%s' = '%s', scope %s", opt.c_str(), val.c_str(), "global");
		}
		return s;
	}

	template<typename T>
	T get(const std::string& opt, option_scope scope, const T& default_value)
	{
		sel::Selector s = L_[domain_.c_str()][name_.c_str()][opt.c_str()];

		if (!s && scope >= scope_domain)
			s = L_[domain_.c_str()][opt.c_str()];
		if (!s && scope >= scope_global)
			s = L_[opt.c_str()];
		if (!s)
			return default_value;
		return s;
	}

private:
	void usage();

	std::string domain_;
	std::string name_;
	sel::State L_;
};

