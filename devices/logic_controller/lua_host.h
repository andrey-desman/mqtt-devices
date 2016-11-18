#pragma once

#include "lua_timer.h"
#include "mqtt_connection.h"

#include "selene.h"

class lua_host
{
public:
	lua_host(mqtt_connection& conn, ev::loop_ref& loop, const std::string& main_dir, const std::string& config_file);

	lua_host(const lua_host&) = delete;
	lua_host& operator = (const lua_host&) = delete;

	~lua_host();

private:
	void subscribe();
	void handle_event(const std::string& topic, mqtt::message_ptr msg);

	void send_switch_command(std::string s, int channel, std::string command);

private:
	mqtt_connection& connection_;
	sel::State L_;
};

