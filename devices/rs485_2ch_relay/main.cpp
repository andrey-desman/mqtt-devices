#include "mqtt_switch_app.h"
#include "rs485_2ch_relay.h"

#include "options.h"

int main(int argc, char* argv[])
{
	std::string broker;
	std::string client_id;
	std::string server_addr;
	std::string serial_path;
	uint16_t server_port;
	uint16_t slave_addr;

	try
	{
		options opt("rs485_2ch", argc, argv);

		broker = opt.get("broker", options::scope_global, std::string("localhost"));
		client_id = opt.get_name();
		server_addr = opt.get<std::string>("modbus_server_addr", options::scope_domain, "localhost");
		server_port = opt.get<int>("modbus_server_port", options::scope_domain, 502);
		serial_path = opt.get<std::string>("modbus_device", options::scope_domain, std::string());
		slave_addr = opt.get<int>("modbus_slave_addr", options::scope_device);
	}
	catch (const std::exception&)
	{
		return 1;
	}

	ev::default_loop loop;

	if (serial_path.empty())
	{
		rs485_2ch_relay relay(server_addr, server_port, slave_addr);
		mqtt_switch_app app(loop, broker, client_id, relay);
		app.run();
	}
	else
	{
		rs485_2ch_relay relay(serial_path, slave_addr);
		mqtt_switch_app app(loop, broker, client_id, relay);
		app.run();
	}

	return 0;
}

