#include "mqtt_switch_app.h"
#include "sample_switch.h"

#include "options.h"

int main(int argc, char* argv[])
{
	namespace po = boost::program_options;

	std::string broker;
	std::string client_id;

	{
		options opt(client_id, broker);
		po::variables_map vm;

		if (!opt.parse(argc, argv, vm))
		{
			return 1;
		}
	}

	ev::default_loop loop;
	sample_switch<5> s;

	mqtt_switch_app app(loop, broker, client_id, s);

	app.run();

	return 0;
}

