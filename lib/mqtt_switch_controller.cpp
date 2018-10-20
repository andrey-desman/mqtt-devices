#include "mqtt_switch_controller.h"
#include "mqtt_connection.h"
#include "iswitch.h"
#include "util.h"
#include "log.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/lexical_cast.hpp>

namespace
{

size_t increase(size_t value, size_t delta)
{
	return std::min<size_t>(value + delta, 100);
}

size_t decrease(size_t value, size_t delta)
{
	return value < delta ? 0 : value - delta;
}

}

mqtt_switch_controller::mqtt_switch_controller(iswitch& s, mqtt_connection& c)
	: switch_(s)
	, connection_(c)
{
	using namespace std::placeholders;
	c.set_on_connected(std::bind(&mqtt_switch_controller::subscribe, this));
	c.set_on_message(std::bind(&mqtt_switch_controller::handle_command, this, _1));
}

mqtt_switch_controller::~mqtt_switch_controller()
{
	connection_.set_on_connected(mqtt_connection::on_connected_handler());
	connection_.set_on_message(mqtt_connection::on_message_handler());

	noexception([this]
		{ connection_.client().unsubscribe(connection_.client().get_client_id() + "/switch/+/command")->wait(); });
}

void mqtt_switch_controller::subscribe()
{
	noexception([this]
		{ connection_.client().subscribe(connection_.client().get_client_id() + "/switch/+/command", 0)->wait(); });
}

size_t mqtt_switch_controller::process_command(const std::string& command, size_t value)
{
	std::vector<std::string> parts;
	boost::split(parts, command, &isspace, boost::token_compress_on);

	const std::string& cmd = parts.at(0);
	auto get_delta = [&parts] { return parts.size() > 1 ? boost::lexical_cast<size_t>(parts[1]) : default_delta; };

		 if (cmd == "inc")    return increase(value, get_delta());
	else if (cmd == "dec")    return decrease(value, get_delta());
	else if (cmd == "on")     return 100;
	else if (cmd == "close")  return 100;
	else if (cmd == "off")    return 0;
	else if (cmd == "open")   return 0;
	else if (cmd == "toggle") return value ? 0 : 100;
	else if (cmd == "nop")    return value;
	else if (cmd == "set")    return boost::lexical_cast<size_t>(parts.at(1));
	else return boost::lexical_cast<size_t>(cmd);
}

void mqtt_switch_controller::handle_command(mqtt::const_message_ptr msg)
{
	try
	{
		std::vector<std::string> parts;
		boost::split(parts, msg->get_topic(), [](char c) { return c == '/'; });

		size_t channel = boost::lexical_cast<size_t>(parts.at(2));

		if (channel >= switch_.get_channel_count())
		{
			LOG(error, "invalid channel %zd (%zd available)", channel, switch_.get_channel_count());
			return;
		}

		size_t channel_state = switch_.get_channel_state(channel);
		size_t value = process_command(msg->get_payload(), channel_state);
		switch_.set_channel_state(channel, value);
		LOG(info, "channel %zu: %zu -> %zu", channel, channel_state, value);

		parts[3] = "state";
		std::string state = boost::lexical_cast<std::string>(switch_.get_channel_state(channel));
		connection_.client().publish(boost::join(parts, "/"), state.c_str(), state.size(), 0, true)->wait();

	}
	catch (const boost::bad_lexical_cast& e)
	{
		LOG(error, "Bad cast: %s", e.what());
	}
	catch (const std::out_of_range& e)
	{
		LOG(error, "Invalid command: %s", msg->get_payload());
	}
	catch (const mqtt::exception& e)
	{
		LOG(error, "MQTT exception: %s", e.what());
	}
	catch (...)
	{
		LOG(error, "Unknown error");
	}
}

