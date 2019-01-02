#pragma once

#include "iswitch.h"
#include "serial.h"

#include <memory>
#include <stdexcept>
#include <string>

class kmtronic_usb_relay: public iswitch
{
	const size_t CHANNEL_COUNT = 8;

public:
	kmtronic_usb_relay(const std::string& dev_path);

	virtual size_t get_channel_count()
	{
		return CHANNEL_COUNT;
	}

	virtual size_t get_channel_state(size_t channel)
	{
		if (channel >= CHANNEL_COUNT)
		{
			throw std::runtime_error("kmtronic usb relay: channel is out of bounds");
		}
		return state_[channel] * 100;
	}

	virtual void set_channel_state(size_t channel, size_t value);

private:
	kmtronic_usb_relay(const kmtronic_usb_relay&) = delete;
	kmtronic_usb_relay& operator =(const kmtronic_usb_relay&) = delete;

	bool state_[8];
	serial device_;
};

