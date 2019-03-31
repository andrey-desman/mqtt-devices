#include "modbus_rtu_device.h"
#include "util.h"

#include <stdexcept>

#include <fcntl.h>

modbus_rtu_device::modbus_rtu_device(const std::string& tty_path, uint16_t slave_addr)
	: modbus_(modbus_new_rtu(tty_path.c_str(), 9600, 'N', 8, 1))
{
	modbus_error_recovery_mode mode = modbus_error_recovery_mode(MODBUS_ERROR_RECOVERY_PROTOCOL);

	check(modbus_, "Failed to create modbus context");
	modbus_set_debug(modbus_, 1);
	check(modbus_set_error_recovery(modbus_, mode), "Failed to set recovery mode");
	check(modbus_set_slave(modbus_, slave_addr), "Failed to set slave address");

	tty_.open(tty_path.c_str(), O_RDWR);
	modbus_set_socket(modbus_, tty_.fd());
}

modbus_rtu_device::~modbus_rtu_device()
{
	modbus_set_socket(modbus_, -1);
	modbus_free(modbus_);
}

uint16_t modbus_rtu_device::read_register(uint16_t addr)
{
	uint16_t out = 0;
	serial::scoped_lock l(&tty_);
	check(modbus_read_registers(modbus_, addr, 1, &out), "failed to read register");
	return out;
}

void modbus_rtu_device::write_register(uint16_t addr, uint16_t data)
{
	serial::scoped_lock l(&tty_);
	check(modbus_write_register(modbus_, addr, data), "failed to write register");
}
