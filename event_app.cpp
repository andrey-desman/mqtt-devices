#include "mqtt_app.h"
#include "options.h"
#include "mqtt_evdev.h"

#include <iostream>

int main(int argc, char* argv[])
{
	namespace po = boost::program_options;

	std::string broker;
	std::string client_id;
	std::string event_path;
	std::vector<int> repeat;
	bool grab = false;

	options opt(client_id, broker);

	{
		po::options_description ev_opt("Event");
		ev_opt.add_options()
			("evdev,e", po::value<std::string>(&event_path)->required(), "Path to input device event node")
			("grab,g", po::bool_switch(&grab)->default_value(false), "Grab device for exclusive access")
			("repeat,r", po::value<std::vector<int> >(&repeat)->multitoken(), "Delay/repeat interval in milliseconds, e.g. -r 1200 600")
		;
		opt.add(ev_opt);

		po::variables_map vm;

		if (!opt.parse(argc, argv, vm))
		{
			return 1;
		}

		if (repeat.size() != 0 && repeat.size() != 2)
		{
			std::cout << "Error: expected 2 values for --repeat option, e.g. -r 1000 500" << std::endl;
			opt.help();
			return 1;
		}
	}

	ev::default_loop loop;
	mqtt_app app(loop, broker, client_id);
	sleep(1);
	mqtt_evdev evdev(event_path, loop, app.client(), repeat, grab);

	app.run();

	return 0;
}

