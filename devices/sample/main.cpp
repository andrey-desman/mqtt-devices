#include "mqtt_switch_app.h"
#include "sample_switch.h"

#include "options.h"

int main(int argc, char* argv[])
{
	std::string broker;
	std::string client_id;

	try
	{
		options opt("sample", argc, argv);

		broker = opt.get("broker", options::scope_global, std::string("localhost"));
		client_id = opt.get_name();
	}
	catch (const std::exception& e)
	{
		return 1;
	}

	ev::default_loop loop;
	sample_switch<5> s;

	mqtt_switch_app app(loop, broker, client_id, s);

	app.run();

	return 0;
}

