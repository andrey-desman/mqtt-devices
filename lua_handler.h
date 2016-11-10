#pragma once

#include "mqtt_connection.h"
#include "util.h"

#include "selene.h"

#include <boost/algorithm/string/split.hpp>
#include <iostream>

class lua_handler
{
public:
	lua_handler(mqtt_connection& conn, const std::string& main_dir, const std::string& config_file)
		: connection_(conn)
		, L_(true)
	{
		using namespace std::placeholders;
		conn.set_on_connected(std::bind(&lua_handler::subscribe, this));
		conn.set_on_message(std::bind(&lua_handler::handle_event, this, _1, _2));

		L_["bus"].SetObj(*this, "send_switch_command", &lua_handler::send_switch_command);
		L_["MAIN_DIR"] = main_dir + "/?;" + main_dir + "/?.lua";
		L_["EV_KEY_RELEASE"] = 0;
		L_["EV_KEY_PRESS"] = 1;
		L_["EV_KEY_REPEAT"] = 2;

		L_.Load(main_dir + "/Main.lua");
		L_.Load(config_file);
	}

	~lua_handler()
	{
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
		const char* function;

		if (parts.at(1) == "switch")
		{
			function = "on_switch_state_changed";
		}
		else if (parts.at(1) == "event")
		{
			function = "on_key_event";
		}
		else
		{
			return;
		}

		L_[function](parts.at(0), boost::lexical_cast<int>(parts.at(2)), boost::lexical_cast<int>(msg->get_payload()));
	}

	void send_switch_command(std::string s, int channel, std::string command)
	{
		noexception([&]()
		{
			connection_.client().publish(s + "/switch/" + boost::lexical_cast<std::string>(channel) + "/command",
				command.c_str(), command.size(), 0, false)->wait_for_completion();
		});
	}

	mqtt_connection& connection_;
	sel::State L_;
};

