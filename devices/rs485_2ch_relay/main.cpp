#include "mqtt_switch_app.h"
#include "rs485_2ch_relay.h"

#include "options.h"

int main(int argc, char* argv[])
{
	namespace po = boost::program_options;

	std::string broker;
	std::string client_id;
	std::string server_addr;
	uint16_t server_port;
	uint16_t slave_addr;

	{
		options opt(client_id, broker);

		po::options_description mbus_opt("Modbus setings");
		mbus_opt.add_options()
			("server,s", po::value<std::string>(&server_addr)->default_value("127.0.0.1"), "Modbus server address")
			("port,p", po::value<uint16_t>(&server_port)->default_value(502), "Modbus server port")
			("slave,l", po::value<uint16_t>(&slave_addr)->required(), "2ch relay slave address")
		;
		opt.add(mbus_opt);

		po::variables_map vm;

		if (!opt.parse(argc, argv, vm))
		{
			return 1;
		}
	}

	ev::default_loop loop;
	rs485_2ch_relay relay(server_addr, server_port, slave_addr);

	mqtt_switch_app app(loop, broker, client_id, relay);

	app.run();

	return 0;
}

