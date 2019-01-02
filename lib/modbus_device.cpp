#include "modbus_tcp_device.h"
#include "modbus_rtu_device.h"

modbus_device* modbus_device::create_tcp_device(const std::string& server_addr, uint16_t port, uint16_t slave_addr)
{
	return new modbus_tcp_device(server_addr, port, slave_addr);
}

modbus_device* modbus_device::create_rtu_device(const std::string& tty_path, uint16_t slave_addr)
{
	return new modbus_rtu_device(tty_path, slave_addr);
}
