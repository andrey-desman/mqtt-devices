#include "mqtt_app.h"
#include "options.h"
#include "lua_host.h"

int main(int argc, char* argv[])
{
	namespace po = boost::program_options;

	std::string broker;
	std::string client_id;
	std::string main_dir;
	std::string config_file;

	options opt(client_id, broker);

	{
		po::options_description ev_opt("Event");
		ev_opt.add_options()
			("lua-path,l", po::value<std::string>(&main_dir)->default_value("/etc/mqttlua"), "Path to main lua handler code")
			("def,d", po::value<std::string>(&config_file)->required(), "Path to controller configuration file")
		;
		opt.add(ev_opt);

		po::variables_map vm;

		if (!opt.parse(argc, argv, vm))
		{
			return 1;
		}
	}

	ev::default_loop loop;
	mqtt_app app(loop, broker, client_id);

	lua_host host(app.connection(), loop, main_dir, config_file);

	app.run();

	return 0;
}

