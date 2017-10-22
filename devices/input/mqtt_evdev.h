#pragma once

#include <ev++.h>
#include <mqtt/async_client.h>

#include <memory>
#include <string>
#include <array>
#include <unordered_map>

#include <linux/input.h>

class mqtt_evdev
{
public:
	mqtt_evdev(const std::string& event, ev::loop_ref& loop, mqtt::async_client& client, std::array<int, 2> repeat, bool grab, bool map_abs);
	mqtt_evdev(const mqtt_evdev&) = delete;
	mqtt_evdev& operator = (const mqtt_evdev&) = delete;

private:
	enum KeyEvent
	{
		KEY_RELEASE,
		KEY_PRESS,
		KEY_REPEAT
	};

	enum
	{
		AXIS_COUNT = ABS_RZ + 1
	};

	struct abs_key
	{
		abs_key(): threshold(), code(), pressed() { }
		int threshold;
		int code;
		bool pressed;
	};

	struct abs_axis
	{
		abs_axis(): remapped() { }
		bool remapped;
		abs_key low;
		abs_key high;
	};

	struct key_repeat_timer: public ev::timer
	{
		int code;
	};

	void process_event(ev::io& io, int revents);
	void process_key(int code, KeyEvent event);
	void process_abs(int code, int value);
	void press_key(abs_key& key, KeyEvent event);
	void process_repeat(ev::timer& timer, int revents);
	void start_repeating(int code);
	void stop_repeating(int code);

private:
	ev::loop_ref& loop_;
	mqtt::async_client& client_;
	std::array<int, 2> repeat_;
	std::array<abs_axis, AXIS_COUNT> abs_axis_;

	int fd_;
	std::shared_ptr<void> fd_guard_;
	std::string path_;
	ev::io event_watcher_;
	std::unordered_map<int, key_repeat_timer> repeat_timers_;
};
