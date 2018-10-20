#include "am82tv.h"
#include "crc16arc.h"

#include <unistd.h>
#include <fcntl.h>

#include <algorithm>
#include <functional>
#include <cstring>

constexpr unsigned char PREFIX = 0x55;

am82tv::am82tv(const std::string& dev_path)
{
	state_ = 0;

	fd_ = open(dev_path.c_str(), O_WRONLY);

	if (fd_ == -1)
	{
		throw std::runtime_error(strerror(errno));
	}

	fd_guard_ = std::shared_ptr<void>(0, std::bind(&close, fd_));
}

void am82tv::set_channel_state(size_t channel, size_t value)
{
	if (channel != 0)
	{
		throw std::runtime_error("am82tv: channel is out of bounds");
	}

	state_ = value;
#if 0
	unsigned char cmd[3] = {0xff, static_cast<unsigned char>(channel + 1), state_[channel]};

	if (write(fd_, &cmd, sizeof(cmd)) == -1)
	{
		throw std::runtime_error(strerror(errno));
	}
#endif
}

// std::vector am82tv::init_command()
// {
	// std::vector<uint8_t> command
// }

void am82tv::append_crc16(std::vector<uint8_t>& command)
{
	uint16_t crc = crc16arc(command.data(), command.size());
	command.push_back(crc);
	command.push_back(crc >> 8);
}

int am82tv::control(Control::Command cmd)
{
	std::vector<uint8_t> command = {PREFIX, 0xfe, 0xfe, Operation::Control, cmd};
	append_crc16(command);

	if (write(fd_, command.data(), command.size()) == command.size())
	{
		
	}
	return false;
}

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

