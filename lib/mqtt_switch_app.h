#pragma once

#include "mqtt_app.h"
#include "mqtt_switch_controller.h"

class mqtt_switch_app: public mqtt_app
{
public:
	mqtt_switch_app(ev::loop_ref& loop, const std::string& uri, const std::string& client_id, iswitch& s)
		: mqtt_app(loop, uri, client_id)
		, controller_(s, this->connection())
	{
	}
	mqtt_switch_app(const mqtt_app&) = delete;
	mqtt_switch_app& operator = (const mqtt_app&) = delete;

private:
	mqtt_switch_controller controller_;
};

