#pragma once

#include <ev++.h>
#include <mqtt/async_client.h>

#include <memory>
#include <string>
#include <vector>

class mqtt_evdev
{
public:
	mqtt_evdev(const std::string& event, ev::loop_ref& loop, mqtt::async_client& client, const std::vector<int>& repeat, bool grab);
	mqtt_evdev(const mqtt_evdev&) = delete;
	mqtt_evdev& operator = (const mqtt_evdev&) = delete;

private:
	void process_event(ev::io& io, int revents);

private:

	int fd_;
	std::shared_ptr<void> fd_guard_;
	std::string path_;

	ev::loop_ref& loop_;
	ev::io event_watcher_;
	mqtt::async_client& client_;
};
