#pragma once

#include <mqtt/async_client.h>

class mqtt_connection;
class iswitch;

class mqtt_switch_controller
{
public:
	enum
	{
		max_state = 100,
		default_delta = 20
	};

	mqtt_switch_controller(iswitch& s, mqtt_connection& c);
	mqtt_switch_controller(const mqtt_switch_controller&) = delete;
	mqtt_switch_controller& operator = (const mqtt_switch_controller&) = delete;
	~mqtt_switch_controller();

private:
	void subscribe();
	size_t process_command(const std::string& command, size_t value);
	void handle_command(mqtt::const_message_ptr msg);

	mqtt_connection& connection_;
	iswitch& switch_;
};

