#pragma once

#include "modbus_device.h"
#include "serial.h"

#include <modbus/modbus.h>

class modbus_rtu_device: public modbus_device
{
public:
	modbus_rtu_device(const std::string& ttyp_path, uint16_t slave_addr);
	~modbus_rtu_device();

	modbus_rtu_device(const modbus_rtu_device&) = delete;
	modbus_rtu_device& operator = (const modbus_rtu_device&) = delete;

	virtual uint16_t read_register(uint16_t addr) override;
	virtual void write_register(uint16_t addr, uint16_t data) override;

private:
	modbus_t* modbus_;
	serial tty_;
};

