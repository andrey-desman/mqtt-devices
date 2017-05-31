#include "mqtt_app.h"
#include "options.h"
#include "lua_host.h"

int main(int argc, char* argv[])
{
	std::string broker;
	std::string client_id;
	std::string main_dir;
	std::string config_file;

	try
	{
		options opt("controller", argc, argv);

		broker = opt.get("broker", options::scope_global, std::string("localhost"));
		client_id = opt.get_name();
		main_dir = opt.get<std::string>("lua_path", options::scope_device, "/etc/mqtt-devices");
		config_file = opt.get<std::string>("config_path", options::scope_device);
	}
	catch (const std::exception&)
	{
		return 1;
	}

	ev::default_loop loop;
	mqtt_app app(loop, broker, client_id);

	lua_host host(app.connection(), loop, main_dir, config_file);

	app.run();

	return 0;
}

