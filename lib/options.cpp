#include "options.h"

#include <unistd.h>

#include <iostream>

options::options(const std::string& domain, int argc, char* argv[])
	: domain_(domain)
{
	int opt;
	std::string cfg = "/etc/mqtt-devices.lua.conf";

	while ((opt = getopt(argc, argv, "c:")) != -1)
	{
		switch (opt) {
		case 'c':
			cfg = optarg;
			break;
		default:
			std::cerr << "Error: unrecognized option - " << (char)opt << std::endl;
			usage();
			throw std::invalid_argument("");
		}
	}

	if (optind >= argc)
	{
		std::cerr << "Error: missing device name" << std::endl;
		usage();
		throw std::invalid_argument("");
	}

	name_ = argv[optind];
	if (!L_.Load(cfg))
	{
		throw std::runtime_error("");
	}
}

void options::usage()
{
	std::cout << "Usage:" << std::endl;
	std::cout << "binary_name [-c config_file] device_name" << std::endl;
	std::cout << "   config_file - path to lua config file, default " << "/etc/mqtt-devices.lua.conf" << std::endl;
	std::cout << "   device_name - device name (aka mqtt client id) to serve" << std::endl;
}
