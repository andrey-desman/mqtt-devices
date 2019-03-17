#pragma once

#include "modbus_device.h"
#include "iswitch.h"

#include <memory>
#include <stdexcept>
#include <string>

class hd0742m_dimmer: public iswitch
{
	static constexpr size_t CHANNEL_COUNT = 2;

public:
	hd0742m_dimmer(const std::string& server_addr, uint16_t port, uint16_t slave_addr);
	hd0742m_dimmer(const std::string& serial_path, uint16_t slave_addr);

	hd0742m_dimmer(const hd0742m_dimmer&) = delete;
	hd0742m_dimmer& operator =(const hd0742m_dimmer&) = delete;

	virtual size_t get_channel_count()
	{
		return CHANNEL_COUNT;
	}

	virtual size_t get_channel_state(size_t channel)
	{
		if (channel >= CHANNEL_COUNT)
		{
			throw std::runtime_error("hd0742m dimmer: channel is out of bounds");
		}
		return state_[channel];
	}

	virtual void set_channel_state(size_t channel, size_t value);

private:
	void read_state();

	static size_t scale(uint16_t hw_value);
	static uint16_t rscale(size_t value);

private:
	std::unique_ptr<modbus_device> device_;
	size_t state_[CHANNEL_COUNT]{};
};

