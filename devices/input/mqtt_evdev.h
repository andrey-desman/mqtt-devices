#pragma once

#include <ev++.h>
#include <mqtt/async_client.h>

#include <memory>
#include <string>
#include <array>
#include <unordered_map>

class mqtt_evdev
{
public:
	mqtt_evdev(const std::string& event, ev::loop_ref& loop, mqtt::async_client& client, std::array<int, 2> repeat, bool grab);
	mqtt_evdev(const mqtt_evdev&) = delete;
	mqtt_evdev& operator = (const mqtt_evdev&) = delete;

private:
	enum
	{
		KEY_RELEASE,
		KEY_PRESS,
		KEY_REPEAT
	};

	struct key_repeat_timer: public ev::timer
	{
		int code;
	};

	void process_event(ev::io& io, int revents);
	void process_repeat(ev::timer& timer, int revents);
	void start_repeating(int code);
	void stop_repeating(int code);

private:
	ev::loop_ref& loop_;
	mqtt::async_client& client_;
	std::array<int, 2> repeat_;

	int fd_;
	std::shared_ptr<void> fd_guard_;
	std::string path_;
	ev::io event_watcher_;
	std::unordered_map<int, key_repeat_timer> repeat_timers_;
};
