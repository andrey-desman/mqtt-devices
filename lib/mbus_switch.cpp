#include "mbus_switch.h"
#include "util.h"

#include <stdexcept>

mbus_switch::mbus_switch(const std::string& server_addr, uint16_t port, uint16_t slave_addr)
	: modbus_(modbus_new_tcp(server_addr.c_str(), port))
{
	modbus_error_recovery_mode mode = modbus_error_recovery_mode(MODBUS_ERROR_RECOVERY_PROTOCOL);

	check(modbus_, "Failed to create modbus context");
	modbus_set_debug(modbus_, 1);
	check(modbus_set_error_recovery(modbus_, mode), "Failed to set recovery mode");
	check(modbus_set_slave(modbus_, slave_addr), "Failed to set slave address");
}

mbus_switch::~mbus_switch()
{
	modbus_close(modbus_);
	modbus_free(modbus_);
}

uint16_t mbus_switch::read_register(uint16_t addr)
{
	uint16_t out = 0;
	if (modbus_read_registers(modbus_, addr, 1, &out) == -1)
	{
		modbus_close(modbus_);
		modbus_connect(modbus_);
		check(modbus_read_registers(modbus_, addr, 1, &out), "Failed to read register");
	}
	return out;
}

void mbus_switch::write_register(uint16_t addr, uint16_t data)
{
	if (modbus_write_register(modbus_, addr, data) == -1)
	{
		modbus_close(modbus_);
		modbus_connect(modbus_);
		check(modbus_write_register(modbus_, addr, data), "Failed to write register");
	}
}
