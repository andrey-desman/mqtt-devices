#include "mqtt_app.h"
#include "options.h"
#include "mqtt_evdev.h"

#include <iostream>

int main(int argc, char* argv[])
{
	std::string broker;
	std::string client_id;
	std::string evdev_path;
	std::array<int, 2> repeat;
	bool grab;

	try
	{
		options opt("hd0742m", argc, argv);

		broker = opt.get("broker", options::scope_global, std::string("localhost"));
		client_id = opt.get_name();
		evdev_path = opt.get<std::string>("device", options::scope_device);
		repeat[0] = opt.get<int>("repeat_delay", options::scope_domain, 0);
		repeat[1] = opt.get<int>("repeat_rate", options::scope_domain, 0);
		grab = opt.get<bool>("grab", options::scope_domain, false);
	}
	catch (const std::exception&)
	{
		return 1;
	}

	ev::default_loop loop;
	mqtt_app app(loop, broker, client_id);
	// sleep(1);
	mqtt_evdev evdev(evdev_path, loop, app.client(), repeat, grab);

	app.run();

	return 0;
}

