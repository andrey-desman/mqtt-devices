#include "lua_host.h"
#include "lua_timer.h"
#include "util.h"
#include "log.h"

#include <boost/algorithm/string/split.hpp>

lua_host::lua_host(mqtt_connection& conn, ev::loop_ref& loop, const std::string& main_dir, const std::string& config_file)
	: connection_(conn)
	, l_(luaL_newstate())
	, L_(l_)
{
	luaL_openlibs(l_);
	using namespace std::placeholders;
	conn.set_on_connected(std::bind(&lua_host::subscribe, this));
	conn.set_on_message(std::bind(&lua_host::handle_event, this, _1));

	LuaTimer::set_loop(&loop);

	L_["MAIN_DIR"] = main_dir + "/?;" + main_dir + "/?.lua";
	L_["EV_KEY_RELEASE"] = 0;
	L_["EV_KEY_PRESS"] = 1;
	L_["EV_KEY_REPEAT"] = 2;

	L_["bus"].SetObj(*this,
		"send_switch_command", &lua_host::send_switch_command,
		"log", &lua_host::log);

	L_["Timer"].SetClass<LuaTimer, sel::function<void ()> >(
		"start", &LuaTimer::start,
		"stop", &LuaTimer::stop);

	L_.Load(main_dir + "/Main.lua");
	L_.Load(config_file);
}

lua_host::~lua_host()
{
	connection_.set_on_connected(mqtt_connection::on_connected_handler());
	connection_.set_on_message(mqtt_connection::on_message_handler());

	noexception([this]
	{
		connection_.client().unsubscribe("+/switch/+/state")->wait();
		connection_.client().unsubscribe("+/event/+")->wait();
	});
	lua_close(l_);
}

void lua_host::subscribe()
{
	noexception([this]
	{
		connection_.client().subscribe("+/switch/+/state", 0);
		connection_.client().subscribe("+/event/+", 0);
	});

}

void lua_host::handle_event(mqtt::const_message_ptr msg)
{
	std::vector<std::string> parts;
	boost::split(parts, msg->get_topic(), [](char c) { return c == '/'; });
	const char* function;

	if (parts.at(1) == "switch")
	{
		function = "on_switch_state_changed";
	}
	else if (parts.at(1) == "event")
	{
		function = "on_key_event";
	}
	else
	{
		return;
	}

	L_[function](parts.at(0), boost::lexical_cast<int>(parts.at(2)), boost::lexical_cast<int>(msg->get_payload()));
}

void lua_host::send_switch_command(std::string s, int channel, std::string command)
{
	noexception([&]()
	{
		connection_.client().publish(s + "/switch/" + boost::lexical_cast<std::string>(channel) + "/command",
			command.c_str(), command.size(), 0, false)->wait();
	});
}

void lua_host::log(std::string s)
{
	lua_Debug info;
	int level = 0;
	if (lua_getstack(l_, 1, &info))
	{
		lua_getinfo(l_, "nSl", &info);
		LOGL(info, logger::file_name(info.short_src), info.currentline, (info.name ? info.name : "<unknown>"), "%s", s.c_str());
	}
}


