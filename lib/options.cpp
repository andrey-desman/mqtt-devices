#include "options.h"
#include "log.h"

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
			throw std::invalid_argument("Invalid option");
		}
	}

	if (optind >= argc)
	{
		std::cerr << "Error: missing device name" << std::endl;
		usage();
		throw std::invalid_argument("");
	}

	name_ = argv[optind];

	logger::init(argv[0], argv[optind]);

	LOG(info, "client id '%s'", name_.c_str());
	LOG(info, "domain '%s'", domain.c_str());
	LOG(info, "config '%s'", cfg.c_str());
	if (!L_.Load(cfg))
	{
		LOG(error, "failed to load config from '%s'", cfg.c_str());
		std::cerr << "Failed to load config file" << std::endl;
		throw std::runtime_error("Failed to load config file");
	}
}

void options::usage()
{
	std::cout << "Usage:" << std::endl;
	std::cout << "binary_name [-c config_file] device_name" << std::endl;
	std::cout << "   config_file - path to lua config file, default " << "/etc/mqtt-devices.lua.conf" << std::endl;
	std::cout << "   device_name - device name (aka mqtt client id) to serve" << std::endl;
}
