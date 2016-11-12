#include "kmtronic_usb_relay.h"

#include <unistd.h>
#include <fcntl.h>

#include <algorithm>
#include <cstring>

kmtronic_usb_relay::kmtronic_usb_relay(const std::string& dev_path)
{
	std::fill(&state_[0], &state_[CHANNEL_COUNT], 0);

	fd_ = open(dev_path.c_str(), O_WRONLY);

	if (fd_ == -1)
	{
		throw std::runtime_error(strerror(errno));
	}

	fd_guard_ = std::shared_ptr<void>(0, std::bind(&close, fd_));
}

void kmtronic_usb_relay::set_channel_state(size_t channel, size_t value)
{
	if (channel >= CHANNEL_COUNT)
	{
		throw std::runtime_error("kmtronic usb relay: channel is out of bounds");
	}

	state_[channel] = value;
	unsigned char cmd[3] = {0xff, static_cast<unsigned char>(channel + 1), state_[channel]};

	if (write(fd_, &cmd, sizeof(cmd)) == -1)
	{
		throw std::runtime_error(strerror(errno));
	}
}

