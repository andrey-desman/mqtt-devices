#include "mqtt_connection.h"
#include "util.h"

#include <iostream>

mqtt_connection::mqtt_connection(mqtt::async_client& client, ev::loop_ref& loop, call_queue& caller)
	: client_(client)
	, loop_(loop)
	, caller_(caller)
{
	client_.set_callback(*this);
}

mqtt_connection::~mqtt_connection()
{
	noexception([this]() {
		client_.disconnect()->wait_for_completion(); });
}

void mqtt_connection::connect()
{
	try
	{
		client_.connect()->wait_for_completion();
		if (on_connected_)
		{
			caller_.post(on_connected_);
		}
	}
	catch (const mqtt::exception& e)
	{
		if (!connect_timer_)
		{
			connect_timer_.reset(new ev::timer(loop_));
			connect_timer_->set<mqtt_connection, &mqtt_connection::connect_timer>(this);
			connect_timer_->start(5, 5);
		}
		return;
	}

	connect_timer_.reset();
}

void mqtt_connection::connect_timer(ev::timer& t, int revents)
{
	connect();
}

// mqtt::callback
void mqtt_connection::connection_lost(const std::string& cause)
{
	if (on_disconnected_)
	{
		caller_.post(on_disconnected_);
	}
	caller_.post(std::move(std::bind(&mqtt_connection::connect, this)));
}

void mqtt_connection::message_arrived(const std::string& topic, mqtt::message_ptr msg)
{
	if (on_message_)
	{
		caller_.post(std::bind(on_message_, topic, msg));
	}
}

void mqtt_connection::delivery_complete(mqtt::idelivery_token_ptr tok)
{
}

