#include "rs485_2ch_relay.h"

#include <algorithm>

rs485_2ch_relay::rs485_2ch_relay(const std::string& server_addr, uint16_t port, uint16_t slave_addr)
	: mbus_switch(server_addr, port, slave_addr)
{
	std::fill(&state_[0], &state_[CHANNEL_COUNT], false);

#if 0
	uint16_t state = read_register(CONTROL_REG);

	state_[0] = scale(state >> 8);
	state_[1] = scale(state & 0xFF);
#endif
}

void rs485_2ch_relay::set_channel_state(size_t channel, size_t value)
{
	static uint16_t cmd[] = { 0x0200, 0x0100 };
	state_[channel] = value;

	write_register(channel + 1, cmd[state_[channel]]);
}

