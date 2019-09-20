#include "mqtt_switch_controller.h"
#include "mqtt_connection.h"
#include "iswitch.h"
#include "util.h"
#include "log.h"

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

size_t mqtt_switch_controller::process_command(std::string_view command, size_t value)
{
	using std::literals::operator""sv;

	std::string_view cmd = shift(command, ' ');
	std::string_view arg = shift(command, ' ');

	auto get_delta = [arg] { return arg.empty() ? default_delta : cast_to_int<size_t>(arg); };

	     if (cmd == "inc"sv)    return increase(value, get_delta());
	else if (cmd == "dec"sv)    return decrease(value, get_delta());
	else if (cmd == "on"sv)     return 100;
	else if (cmd == "close"sv)  return 100;
	else if (cmd == "off"sv)    return 0;
	else if (cmd == "open"sv)   return 0;
	else if (cmd == "toggle"sv) return value ? 0 : 100;
	else if (cmd == "nop"sv)    return value;
	else if (cmd == "stop"sv)   return -1;
	else if (cmd == "set"sv)    return cast_to_int<size_t>(arg);
	else return cast_to_int<size_t>(cmd);
}

void mqtt_switch_controller::handle_command(mqtt::const_message_ptr msg)
{
	using std::literals::operator""sv;

	try
	{
		static_assert(std::is_reference_v<decltype(msg->get_topic())>, "Won't create string_view from non-reference type");
		std::string_view topic = msg->get_topic();
		std::string_view state_topic = topic;

		shift(topic, '/');
		shift(topic, '/');
		size_t channel = cast_to_int<size_t>(shift(topic, '/'));

		if (channel >= switch_.get_channel_count())
		{
			LOG(error, "invalid channel %zd (%zd available)", channel, switch_.get_channel_count());
			return;
		}

		size_t channel_state = switch_.get_channel_state(channel);
		size_t value = process_command(msg->get_payload(), channel_state);
		switch_.set_channel_state(channel, value);
		LOG(info, "channel %zu: %zu -> %zu", channel, channel_state, value);

		pop(state_topic, '/');

		std::string t(state_topic);
		t.append("state"sv);

		std::string state = std::to_string(switch_.get_channel_state(channel));
		connection_.client().publish(t, state.c_str(), state.size(), 0, true)->wait();

	}
	catch (const std::exception& e)
	{
		LOG(error, "%s", e.what());
	}
	catch (...)
	{
		LOG(error, "Unknown error");
	}
}

