#pragma once

#include "mqtt_connection.h"
#include "call_queue.h"

#include "mqtt/async_client.h"
#include "ev++17.h"

class mqtt_app
{
public:
	mqtt_app(ev::loop_ref&, const std::string& uri, const std::string& client_id);
	mqtt_app(const mqtt_app&) = delete;
	mqtt_app& operator = (const mqtt_app&) = delete;

	void run();

	ev::loop_ref& loop()
	{
		return loop_;
	}

	mqtt::async_client& client()
	{
		return client_;
	}

	mqtt_connection& connection()
	{
		return connection_;
	}

private:
	ev::loop_ref& loop_;
	call_queue caller_;
	ev::sig int_watcher_;
	ev::sig term_watcher_;
	mqtt::async_client client_;
	mqtt_connection connection_;
};

