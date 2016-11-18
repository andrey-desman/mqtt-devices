#pragma once

#include "mbus_switch.h"

#include <stdexcept>
#include <string>

class rs485_2ch_relay: public mbus_switch
{
	static const size_t CHANNEL_COUNT = 2;

public:
	rs485_2ch_relay(const std::string& server_addr, uint16_t port, uint16_t slave_addr);

	rs485_2ch_relay(const rs485_2ch_relay&) = delete;
	rs485_2ch_relay& operator =(const rs485_2ch_relay&) = delete;

	virtual size_t get_channel_count()
	{
		return CHANNEL_COUNT;
	}

	virtual size_t get_channel_state(size_t channel)
	{
		return state_[channel] * 100;
	}

	virtual void set_channel_state(size_t channel, size_t value);

private:
	bool state_[CHANNEL_COUNT];
};

