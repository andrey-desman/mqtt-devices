#include "lua_host.h"
#include "lua_timer.h"
#include "util.h"
#include "log.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>

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

	L_["Log"].SetObj(*this, "log", &lua_host::log);

	L_["Mqtt"].SetObj(*this,
		"subscribe", &lua_host::mqtt_subscribe,
		"post", &lua_host::mqtt_post);

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
		for (auto const& i: subscriptions_)
			connection_.client().unsubscribe(i)->wait();
	});
	lua_close(l_);
}

void lua_host::subscribe()
{
	noexception([this]
	{
		for (auto const& i: subscriptions_)
			connection_.client().subscribe(i, 0);
	});

}

void lua_host::handle_event(mqtt::const_message_ptr msg)
{
	L_["Mqtt"]["handle_message"](msg->get_topic(), msg->get_payload());
}

void lua_host::mqtt_post(std::string topic, std::string payload, bool retained)
{
	noexception([&]()
	{
		connection_.client().publish(topic, payload.c_str(), payload.length(), 0, retained)->wait();
	});
}

void lua_host::mqtt_subscribe(std::string topic)
{
	noexception([&]()
	{
		subscriptions_.push_back(topic);
		connection_.client().subscribe(topic, 0);
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


