#pragma once

#include "mqtt_connection.h"
#include "util.h"

extern "C" {
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>
}

#include <boost/algorithm/string/split.hpp>
#include <iostream>

class lua_handler
{
public:
	lua_handler(mqtt_connection& conn, const std::string& file_name)
		: connection_(conn)
	{
		using namespace std::placeholders;
		conn.set_on_connected(std::bind(&lua_handler::subscribe, this));
		conn.set_on_message(std::bind(&lua_handler::handle_event, this, _1, _2));

		L_ = luaL_newstate();
		luaL_openlibs(L_);
		luaopen_mylib(L_, this);

		luaL_loadfile(L_, file_name.c_str());
		lua_pcall(L_, 0, 0, 0);
	}

	~lua_handler()
	{
		lua_close(L_);

		connection_.set_on_connected(std::function<void ()>());
		connection_.set_on_message(std::function<void (const std::string&, mqtt::message_ptr)>());

		noexception([this]
		{
			connection_.client().unsubscribe("+/switch/+/state")->wait_for_completion();
			connection_.client().unsubscribe("+/event/+")->wait_for_completion();
		});
	}

private:
	void subscribe()
	{
		noexception([this]
		{
			connection_.client().subscribe("+/switch/+/state", 0);
			connection_.client().subscribe("+/event/+", 0);
		});

	}

	void handle_event(const std::string& topic, mqtt::message_ptr msg)
	{
		std::vector<std::string> parts;
		boost::split(parts, topic, [](char c) { return c == '/'; });

		if (parts.at(1) == "switch")
		{
			lua_getglobal(L_, "on_switch_state_changed");
		}
		else if (parts.at(1) == "event")
		{
			lua_getglobal(L_, "on_key_event");
		}
		else
		{
			return;
		}

		lua_pushstring(L_, parts.at(0).c_str());
		lua_pushnumber(L_, boost::lexical_cast<int>(parts.at(2).c_str()));
		lua_pushnumber(L_, boost::lexical_cast<int>(msg->get_payload()));
		lua_pcall(L_, 3, 0, 0);
	}

	void send_switch_command(const std::string& s, size_t channel, const std::string& command)
	{
		noexception([&]()
		{
			connection_.client().publish(s + "/switch/" + boost::lexical_cast<std::string>(channel) + "/command",
				command.c_str(), command.size(), 0, false)->wait_for_completion();
		});
	}

	static int send_switch_command(lua_State *L)
	{
		lua_handler* this_ = static_cast<lua_handler*>(lua_touserdata(L, lua_upvalueindex(1)));
		std::string s = lua_tostring(L, -3);
		size_t channel = lua_tointeger(L, -2);
		std::string command = lua_tostring(L, -1);

		this_->send_switch_command(s, channel, command);
		return 0;
	}

	int luaopen_mylib (lua_State *L, lua_handler* h)
	{
		static const struct luaL_Reg bus [] =
		{
			{"send_switch_command", &send_switch_command},
			{NULL, NULL}  /* sentinel */
		};

		luaL_newlibtable(L, bus);
		lua_pushlightuserdata(L, h);
		luaL_setfuncs(L, bus, 1);
		lua_setglobal(L, "bus");
		return 0;
	}


	mqtt_connection& connection_;
	lua_State* L_;
};

