#include "rs485_2ch_relay.h"

rs485_2ch_relay::rs485_2ch_relay(const std::string& server_addr, uint16_t port, uint16_t slave_addr)
{
	device_.reset(modbus_device::create_tcp_device(server_addr, port, slave_addr));
}

rs485_2ch_relay::rs485_2ch_relay(const std::string& serial_path, uint16_t slave_addr)
{
	device_.reset(modbus_device::create_rtu_device(serial_path, slave_addr));
}

void rs485_2ch_relay::set_channel_state(size_t channel, size_t value)
{
	static constexpr uint16_t cmd[] = { 0x0200, 0x0100 };

	if (channel >= CHANNEL_COUNT)
		throw std::runtime_error("rs485_2ch_relay: channel is out of bounds");

	state_[channel] = value;

	device_->write_register(channel + 1, cmd[state_[channel]]);
}

