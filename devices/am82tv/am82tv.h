#pragma once

#include "iswitch.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class am82tv: public iswitch
{
	struct Operation
	{
		enum Code
		{
			Read = 1,
			Write,
			Control
		};
	};

	struct Control
	{
		enum Command
		{
			Open = 1,
			Close,
			Stop,
			Set,
			Reset = 7,
		};
	};

public:
	am82tv(const std::string& dev_path);

	am82tv(const am82tv&) = delete;
	am82tv& operator =(const am82tv&) = delete;

	virtual size_t get_channel_count()
	{
		return 1;
	}

	virtual size_t get_channel_state(size_t channel)
	{
		if (channel >= 1)
		{
			throw std::runtime_error("kmtronic usb relay: channel is out of bounds");
		}
		return state_;
	}

	virtual void set_channel_state(size_t channel, size_t value);

private:
	void append_crc16(std::vector<uint8_t>& command);
	int control(Control::Command cmd);
	int read_state();

	int state_;
	int fd_;
	std::shared_ptr<void> fd_guard_;
};

