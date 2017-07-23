#include "mqtt_evdev.h"
#include "util.h"

#include <boost/lexical_cast.hpp>

#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <iostream>


mqtt_evdev::mqtt_evdev(const std::string& event, ev::loop_ref& loop, mqtt::async_client& client, std::array<int, 2> repeat, bool grab)
	: loop_(loop)
	, client_(client)
	, repeat_(repeat)
	, path_(client_.get_client_id() + "/event/")
	, event_watcher_(loop)
{
	fd_ = open(event.c_str(), O_RDONLY);

	if (fd_ == -1)
	{
		throw std::runtime_error(strerror(errno));
	}

	fd_guard_ = std::shared_ptr<void>(0, std::bind(&close, fd_));

	fcntl(fd_, F_SETFL, O_NONBLOCK);
	grab && ioctl(fd_, EVIOCGRAB, 1);

	if (repeat_[0] <= 0 || repeat_[1] <= 0)
	{
		repeat_[0] = 0;
		repeat_[1] = 0;
	}

	event_watcher_.set<mqtt_evdev, &mqtt_evdev::process_event>(this);
	event_watcher_.start(fd_, ev::READ);
}

void mqtt_evdev::process_event(ev::io& io, int revents)
{
	input_event ev;
	int r;

	while ((r = read(fd_, &ev, sizeof(ev))) == sizeof(ev))
	{
		if (ev.type != EV_KEY || ev.value == KEY_REPEAT)
			continue;

		noexception([&ev, this](){
			std::string payload = boost::lexical_cast<std::string>(ev.value);
			client_.publish(path_ + boost::lexical_cast<std::string>(ev.code), payload.c_str(), payload.size(), 0, false);
		});

		if (repeat_[0] == 0)
			continue;

		if (ev.value == KEY_PRESS)
			start_repeating(ev.code);
		else
			stop_repeating(ev.code);
	}

	if (r == -1 && errno != EWOULDBLOCK)
		loop_.break_loop();
}

void mqtt_evdev::process_repeat(ev::timer& timer, int revents)
{
	int code = static_cast<key_repeat_timer&>(timer).code;
	noexception([this, code]() {
		std::string payload = "2";
		client_.publish(path_ + boost::lexical_cast<std::string>(code), payload.c_str(), payload.size(), 0, false);
	});
}

void mqtt_evdev::start_repeating(int code)
{
	auto& t = repeat_timers_[code];
	t.code = code;
	t.set(loop_);
	t.set<mqtt_evdev, &mqtt_evdev::process_repeat>(this);
	t.start(repeat_[0] / 1000.0, repeat_[1] / 1000.0);
}

void mqtt_evdev::stop_repeating(int code)
{
	repeat_timers_.erase(code);
}


