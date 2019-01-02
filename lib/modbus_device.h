#pragma once

#include <string>
#include <inttypes.h>

class modbus_device
{
public:
	virtual ~modbus_device() { }

	virtual uint16_t read_register(uint16_t addr) = 0;
	virtual void write_register(uint16_t addr, uint16_t data) = 0;

	static modbus_device* create_tcp_device(const std::string& server_addr, uint16_t port, uint16_t slave_addr);
	static modbus_device* create_rtu_device(const std::string& tty_path, uint16_t slave_addr);
};

