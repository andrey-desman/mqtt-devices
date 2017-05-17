#include "mqtt_evdev.h"

#include <boost/lexical_cast.hpp>

#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

mqtt_evdev::mqtt_evdev(const std::string& event, ev::loop_ref& loop, mqtt::async_client& client, std::array<int, 2> repeat, bool grab)
	: loop_(loop)
	, event_watcher_(loop)
	, client_(client)
{
	fd_ = open(event.c_str(), O_RDONLY);

	if (fd_ == -1)
	{
		throw std::runtime_error(strerror(errno));
	}

	fd_guard_ = std::shared_ptr<void>(0, std::bind(&close, fd_));

	fcntl(fd_, F_SETFL, O_NONBLOCK);
	grab && ioctl(fd_, EVIOCGRAB, 1);
	ioctl(fd_, EVIOCSREP, repeat.data());

	path_ = client_.get_client_id() + "/event/";

	event_watcher_.set<mqtt_evdev, &mqtt_evdev::process_event>(this);
	event_watcher_.start(fd_, ev::READ);
}

void mqtt_evdev::process_event(ev::io& io, int revents)
{
	input_event ev;

	while (read(fd_, &ev, sizeof(ev)) == sizeof(ev))
	{
		if (ev.type == EV_KEY)
		{
			std::string payload = boost::lexical_cast<std::string>(ev.value);

			try
			{
				client_.publish(path_ + boost::lexical_cast<std::string>(ev.code), payload.c_str(), payload.size(), 0, false);
			}
			catch (...)
			{
			}
		}
	}
}

