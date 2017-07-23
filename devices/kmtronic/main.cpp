#include "mqtt_switch_app.h"
#include "kmtronic_usb_relay.h"

#include "options.h"

int main(int argc, char* argv[])
{
	std::string broker;
	std::string client_id;
	std::string dev_path;

	try
	{
		options opt("kmtronic", argc, argv);

		broker = opt.get("broker", options::scope_global, std::string("localhost"));
		client_id = opt.get_name();
		dev_path = opt.get<std::string>("device", options::scope_device);
	}
	catch (const std::exception&)
	{
		return 1;
	}

	ev::default_loop loop;
	kmtronic_usb_relay relay(dev_path);

	mqtt_switch_app app(loop, broker, client_id, relay);

	app.run();

	return 0;
}

