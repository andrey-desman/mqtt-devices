#include "hd0742m_dimmer.h"

#include <algorithm>

namespace
{

const uint16_t CONTROL_REG = 0;
const uint16_t MAX_VALUE = 0x39;

}

hd0742m_dimmer::hd0742m_dimmer(const std::string& server_addr, uint16_t port, uint16_t slave_addr)
	: mbus_switch(server_addr, port, slave_addr)
{
	std::fill(&state_[0], &state_[CHANNEL_COUNT], 0);

	uint16_t state = read_register(CONTROL_REG);

	state_[0] = scale(state >> 8);
	state_[1] = scale(state & 0xFF);
}

size_t hd0742m_dimmer::scale(uint16_t hw_value)
{
	return std::min<size_t>(hw_value * 100 / MAX_VALUE, 100);
}

uint16_t hd0742m_dimmer::rscale(size_t value)
{
	return std::min<uint16_t>(value * MAX_VALUE / 100, MAX_VALUE);
}


void hd0742m_dimmer::set_channel_state(size_t channel, size_t value)
{
	if (channel >= CHANNEL_COUNT)
	{
		throw std::runtime_error("kmtronic usb relay: channel is out of bounds");
	}

	state_[channel] = value;

	write_register(CONTROL_REG, (rscale(state_[0]) << 8) | rscale(state_[1]));
}

