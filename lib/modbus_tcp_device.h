#pragma once

#include "modbus_device.h"
#include <modbus/modbus.h>

class modbus_tcp_device: public modbus_device
{
public:
	modbus_tcp_device(const std::string& server_addr, uint16_t port, uint16_t slave_addr);
	~modbus_tcp_device();

	modbus_tcp_device(const modbus_tcp_device&) = delete;
	modbus_tcp_device& operator = (const modbus_tcp_device&) = delete;

	virtual uint16_t read_register(uint16_t addr) override;
	virtual void write_register(uint16_t addr, uint16_t data) override;

private:
	modbus_t* modbus_;
};

