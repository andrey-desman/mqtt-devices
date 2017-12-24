#pragma once

#include "iswitch.h"

#include <modbus/modbus.h>

#include <string>
#include <inttypes.h>

class mbus_switch: public iswitch
{
public:
	mbus_switch(const std::string& server_addr, uint16_t port, uint16_t slave_addr);
	~mbus_switch();

	mbus_switch(const mbus_switch&) = delete;
	mbus_switch& operator = (const mbus_switch&) = delete;

	uint16_t read_register(uint16_t addr);
	void write_register(uint16_t addr, uint16_t data);

private:
	modbus_t* modbus_;
};

