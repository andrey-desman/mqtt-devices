#pragma once

#include "mqtt/async_client.h"
#include "call_queue.h"

class mqtt_connection: mqtt::callback
{
public:
	mqtt_connection(mqtt::async_client& client, ev::loop_ref& loop, call_queue& caller);
	mqtt_connection(const mqtt_connection&) = delete;
	mqtt_connection& operator = (const mqtt_connection&) = delete;
	~mqtt_connection();

	mqtt::async_client& client()
	{
		return client_;
	}

	void set_on_connected(const std::function<void ()>& on_connected)
	{
		on_connected_ = on_connected;
	}

	void set_on_disconnected(const std::function<void ()>& on_disconnected)
	{
		on_disconnected_ = on_disconnected;
	}

	void set_on_message(const std::function<void (const std::string&, mqtt::message_ptr)>& on_message)
	{
		on_message_ = on_message;
	}

	void connect();

private:
	void connect_timer(ev::timer& t, int revents);

	// mqtt::callback
	void connection_lost(const std::string& cause);
	void message_arrived(const std::string& topic, mqtt::message_ptr msg);
	void delivery_complete(mqtt::idelivery_token_ptr tok);

private:
	mqtt::async_client& client_;
	std::unique_ptr<ev::timer> connect_timer_;
	ev::loop_ref& loop_;
	call_queue& caller_;
	std::function<void ()> on_connected_;
	std::function<void ()> on_disconnected_;
	std::function<void (const std::string&, mqtt::message_ptr)> on_message_;
};

