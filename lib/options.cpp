#include "options.h"

#include <iostream>

namespace po = boost::program_options;

options::options(std::string& client_id, std::string& broker)
{
	po::options_description generic("Generic");
	generic.add_options()
		("client-id,c", po::value<std::string>(&client_id)->required(), "MQTT client id")
		("broker,b", po::value<std::string>(&broker)->default_value("localhost:1883"), "MQTT broker address in form of host[:port]")
		("help,h", "Print this message");

	options_.add(generic);
}

bool options::parse(int argc, char* argv[], po::variables_map& vm)
{
	try
	{
		po::parsed_options parsed = po::command_line_parser(argc, argv).options(options_).run();
		po::store(parsed, vm);
		po::notify(vm);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		std::cout << options_ << std::endl;
		return false;
	}

	if (vm.count("help"))
	{
		std::cout << options_ << std::endl;
		return false;
	}

	return true;
}

void options::help()
{
	std::cout << options_ << std::endl;
}
