#include "mqtt_switch_app.h"
#include "am82tv.h"

#include "options.h"

int main(int argc, char* argv[])
{
	std::string broker;
	std::string client_id;
	std::string dev_path;
	uint16_t slave_addr = 0;
	bool inverse = false;

	try
	{
		options opt("am82tv", argc, argv);

		broker = opt.get("broker", options::scope_global, std::string("localhost"));
		client_id = opt.get_name();
		dev_path = opt.get<std::string>("device", options::scope_domain);
		slave_addr = opt.get<int>("slave_addr", options::scope_device);
		inverse = opt.get<bool>("inverse", options::scope_domain, false);
	}
	catch (const std::exception&)
	{
		return 1;
	}

	ev::default_loop loop;
	am82tv motor(dev_path, slave_addr, inverse);

	mqtt_switch_app app(loop, broker, client_id, motor);

	app.run();

	return 0;
}

