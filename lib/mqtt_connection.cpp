#include "mqtt_connection.h"
#include "util.h"
#include "log.h"

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
		client_.disconnect()->wait(); });
}

void mqtt_connection::connect()
{
	try
	{
		client_.connect()->wait();
		LOG(info, "connected");
		if (on_connected_)
		{
			caller_.post(on_connected_);
		}

		connect_timer_.reset();
	}
	catch (const mqtt::exception& e)
	{
		LOG(error, "connection failed, retrying in %d seconds", 5);
		if (!connect_timer_)
		{
			connect_timer_.reset(new ev::timer(loop_));
			connect_timer_->set<mqtt_connection, &mqtt_connection::connect_timer>(this);
			connect_timer_->start(5, 5);
		}
	}
}

void mqtt_connection::connect_timer(ev::timer& t, int revents)
{
	connect();
}

// mqtt::callback
void mqtt_connection::connection_lost(const std::string& cause)
{
	LOG(error, "disconnected");
	if (on_disconnected_)
	{
		caller_.post(on_disconnected_);
	}
	caller_.post(std::move(std::bind(&mqtt_connection::connect, this)));
}

void mqtt_connection::message_arrived(mqtt::const_message_ptr msg)
{
	LOG(debug, "got message '%s' on '%s'", msg->get_payload().c_str(), msg->get_topic().c_str());
	if (on_message_)
	{
		caller_.post(std::bind(on_message_, msg));
	}
}

