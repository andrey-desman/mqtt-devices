#pragma once

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "mqtt_connection.h"

#include "selene.h"

#include <vector>
#include <string>

class lua_host
{
public:
	lua_host(mqtt_connection& conn, ev::loop_ref& loop, const std::string& main_dir, const std::string& config_file);

	lua_host(const lua_host&) = delete;
	lua_host& operator = (const lua_host&) = delete;

	~lua_host();

private:
	void subscribe();
	void handle_event(mqtt::const_message_ptr msg);

	void mqtt_post(std::string topic, std::string payload, bool retained);
	void mqtt_subscribe(std::string topic);
	void log(std::string s);

private:
	std::vector<std::string> subscriptions_;
	mqtt_connection& connection_;
	lua_State* l_;
	sel::State L_;
};

