#pragma once

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "ev++17.h"
#include <selene.h>

class LuaTimer
{
public:
	LuaTimer(sel::function<void ()> handler)
		: timer_(*loop_)
		, handler_(handler)
	{
		timer_.set<LuaTimer, &LuaTimer::handle_timer>(this);
	}

	static void set_loop(ev::loop_ref* loop)
	{
		loop_ = loop;
	}

	void start(lua_Number timeout)
	{
		timer_.start(timeout);
	}

	void stop()
	{
		timer_.stop();
	}

private:
	void handle_timer(ev::timer&, int)
	{
		handler_();
	}

private:
	static ev::loop_ref* loop_;

	ev::timer timer_;
	sel::function<void ()> handler_;
};

