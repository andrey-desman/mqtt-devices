#include "mqtt_switch_controller.h"
#include "mqtt_connection.h"
#include "iswitch.h"
#include "util.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>

namespace
{

size_t increase(size_t value, boost::optional<size_t> delta)
{
	if (!delta) delta = 20;
	return std::min<size_t>(value + *delta, 100);
}

size_t decrease(size_t value, boost::optional<size_t> delta)
{
	if (!delta) delta = 20;
	return value < *delta ? 0 : value - *delta;
}

}

mqtt_switch_controller::mqtt_switch_controller(iswitch& s, mqtt_connection& c)
	: switch_(s)
	, connection_(c)
{
	using namespace std::placeholders;
	c.set_on_connected(std::bind(&mqtt_switch_controller::subscribe, this));
	c.set_on_message(std::bind(&mqtt_switch_controller::handle_command, this, _1, _2));
}

mqtt_switch_controller::~mqtt_switch_controller()
{
	connection_.set_on_connected(std::function<void ()>());
	connection_.set_on_message(std::function<void (const std::string&, mqtt::message_ptr)>());

	noexception([this]
		{ connection_.client().unsubscribe(connection_.client().get_client_id() + "/switch/+/command")->wait_for_completion(); });
}

void mqtt_switch_controller::subscribe()
{
	noexception([this]
		{ connection_.client().subscribe(connection_.client().get_client_id() + "/switch/+/command", 0)->wait_for_completion(); });
}

size_t mqtt_switch_controller::process_command(const std::string& command, size_t value)
{
	std::vector<std::string> parts;
	boost::split(parts, command, &isspace, boost::token_compress_on);

	const std::string& cmd = parts.at(0);

		 if (cmd == "inc")    return increase(value, converter(parts, 1));
	else if (cmd == "dec")    return decrease(value, converter(parts, 1));
	else if (cmd == "on")     return 100;
	else if (cmd == "off")    return 0;
	else if (cmd == "toggle") return value ? 0 : 100;
	else if (cmd == "nop")    return value;
	else if (cmd == "set")    return converter(parts, 1);
	else return boost::lexical_cast<size_t>(cmd);
}

void mqtt_switch_controller::handle_command(const std::string& topic, mqtt::message_ptr msg)
{
	try
	{
		std::vector<std::string> parts;
		boost::split(parts, topic, [](char c) { return c == '/'; });

		size_t channel = boost::lexical_cast<size_t>(parts.at(2));

		if (channel >= switch_.get_channel_count())
		{
			return;
		}

		switch_.set_channel_state(channel,
			process_command(msg->get_payload(), switch_.get_channel_state(channel)));

		parts[3] = "state";
		std::string state = boost::lexical_cast<std::string>(switch_.get_channel_state(channel));
		connection_.client().publish(boost::join(parts, "/"), state.c_str(), state.size(), 0, true)->wait_for_completion();
		std::cout << "Got command <" << msg->get_payload() << "> on channel " << channel << "\n";

	}
	catch (const boost::bad_lexical_cast& e)
	{
		// syslog(LOG_ERR | LOG_DAEMON, "Invalid channel: %s", "asd");
		std::cout << "Bad cast: " << e.what() << std::endl;
	}
	catch (const std::out_of_range& e)
	{
		std::cout << "Invalid command: " << msg->get_payload() << std::endl;
	}
	catch (const mqtt::exception& e)
	{
		std::cout << "MQTT exception: " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Unknown error" << std::endl;
	}
}

