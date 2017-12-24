#pragma once

#include "mqtt/async_client.h"
#include "call_queue.h"

class mqtt_connection: mqtt::callback
{
public:
	typedef std::function<void ()> on_connected_handler;
	typedef std::function<void ()> on_disconnected_handler;
	typedef std::function<void (mqtt::const_message_ptr)> on_message_handler;

public:
	mqtt_connection(mqtt::async_client& client, ev::loop_ref& loop, call_queue& caller);
	mqtt_connection(const mqtt_connection&) = delete;
	mqtt_connection& operator = (const mqtt_connection&) = delete;
	~mqtt_connection();

	mqtt::async_client& client()
	{
		return client_;
	}

	void set_on_connected(const on_connected_handler& on_connected)
	{
		on_connected_ = on_connected;
	}

	void set_on_disconnected(const on_disconnected_handler& on_disconnected)
	{
		on_disconnected_ = on_disconnected;
	}

	void set_on_message(const on_message_handler& on_message)
	{
		on_message_ = on_message;
	}

	void connect();

private:
	void connect_timer(ev::timer& t, int revents);

	// mqtt::callback
	void connection_lost(const std::string& cause);
	void message_arrived(mqtt::const_message_ptr msg);

private:
	mqtt::async_client& client_;
	std::unique_ptr<ev::timer> connect_timer_;
	ev::loop_ref& loop_;
	call_queue& caller_;
	on_connected_handler on_connected_;
	on_disconnected_handler on_disconnected_;
	on_message_handler on_message_;
};

