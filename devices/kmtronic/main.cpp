#include "mqtt_switch_app.h"
#include "kmtronic_usb_relay.h"

#include "options.h"

int main(int argc, char* argv[])
{
	namespace po = boost::program_options;

	std::string broker;
	std::string client_id;
	std::string dev_path;

	{
		options opt(client_id, broker);

		po::options_description ev_opt("Relay");
		ev_opt.add_options()
			("device-path,d", po::value<std::string>(&dev_path)->required(), "Path to KMTronic USB relay device node")
		;
		opt.add(ev_opt);

		po::variables_map vm;

		if (!opt.parse(argc, argv, vm))
		{
			return 1;
		}
	}

	ev::default_loop loop;
	kmtronic_usb_relay relay(dev_path);

	mqtt_switch_app app(loop, broker, client_id, relay);

	app.run();

	return 0;
}

