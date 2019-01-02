#include "rs485_2ch_relay.h"

rs485_2ch_relay::rs485_2ch_relay(const std::string& server_addr, uint16_t port, uint16_t slave_addr)
{
	device_.reset(modbus_device::create_tcp_device(server_addr, port, slave_addr));
}

void rs485_2ch_relay::set_channel_state(size_t channel, size_t value)
{
	static uint16_t cmd[] = { 0x0200, 0x0100 };
	state_[channel] = value;

	device_->write_register(channel + 1, cmd[state_[channel]]);
}

