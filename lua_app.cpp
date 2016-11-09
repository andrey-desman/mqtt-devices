#include "mqtt_app.h"
#include "options.h"
#include "lua_handler.h"

#include <iostream>

int main(int argc, char* argv[])
{
	namespace po = boost::program_options;

	std::string broker;
	std::string client_id;

	options opt(client_id, broker);

	{
		po::variables_map vm;

		if (!opt.parse(argc, argv, vm))
		{
			return 1;
		}
	}

	ev::default_loop loop;
	mqtt_app app(loop, broker, client_id);

	lua_handler l(app.connection(), "test.lua");

	app.run();

	std::cout << "Exiting\n";

	return 0;
}

