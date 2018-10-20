#include "mqtt_switch_app.h"
#include "am82tv.h"

#include "options.h"

int main(int argc, char* argv[])
{
	std::string broker;
	std::string client_id;
	std::string dev_path;

	try
	{
		options opt("am82tv", argc, argv);

		broker = opt.get("broker", options::scope_global, std::string("localhost"));
		client_id = opt.get_name();
		dev_path = opt.get<std::string>("device", options::scope_device);
	}
	catch (const std::exception&)
	{
		return 1;
	}

	ev::default_loop loop;
	am82tv motor(dev_path);

	mqtt_switch_app app(loop, broker, client_id, motor);

	app.run();

	return 0;
}

