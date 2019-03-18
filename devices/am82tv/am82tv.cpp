#include "am82tv.h"
#include "crc16arc.h"
#include "log.h"

#include <unistd.h>
#include <fcntl.h>

#include <algorithm>
#include <functional>
#include <cstring>

constexpr unsigned char PREFIX = 0x55;

am82tv::am82tv(const std::string& dev_path, uint16_t slave_addr)
	: slave_addr_(slave_addr)
{
	state_ = 0;
	device_.open(dev_path.c_str(), O_WRONLY);
	if (!device_)
		throw std::runtime_error(strerror(errno));
}

void am82tv::set_channel_state(size_t channel, size_t value)
{
	if (channel != 0)
	{
		throw std::runtime_error("am82tv: channel is out of bounds");
	}

	if (value == 0)
		control(Control::Open);
	else if (value == 100)
		control(Control::Close);
	else
		control(Control::Set, value);
	state_ = value;
}

void am82tv::append_crc16(std::vector<uint8_t>& command)
{
	uint16_t crc = crc16arc(command.data(), command.size());
	command.push_back(crc);
	command.push_back(crc >> 8);
}

int am82tv::control(Control::Command cmd, uint8_t value)
{
	std::vector<uint8_t> command = {PREFIX, uint8_t(slave_addr_ >> 8), uint8_t(slave_addr_), Operation::Control, cmd};
	if (cmd == Control::Set)
		command.append(value);
	append_crc16(command);

	serial::scoped_lock l(&device_);

	if (device_.write(command.data(), command.size()) == command.size())
	{
		std::vector<uint8_t> reply(command.size());
		if (device_.read_exact(&reply[0], reply.size()) && reply == command)
			return true;
		else
			LOG(error, "Failed to read response from am82tv");
	}
	return false;
}

#if 0
int am82tv::read_state()
{
	unsigned char cmd[] = {PREFIX, 0xfe, 0xfe, 1, 2, 1, 0, 0};

	unsigned short crc = crc16arc(cmd, sizeof(cmd) - 2);
	cmd[sizeof(cmd) - 2] = crc & 0xff;
	cmd[sizeof(cmd) - 1] = (crc >> 8) & 0xff;

	if (write(fd_, cmd, sizeof(cmd)) == -1)
	{
		throw std::runtime_error(strerror(errno));
	}
}
#endif

