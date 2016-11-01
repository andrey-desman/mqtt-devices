#pragma once

#include <list>
#include <mutex>
#include <memory>
#include <functional>

#include <ev++.h>

class call_queue
{
public:
	call_queue(ev::loop_ref& loop)
	: async_(loop)
	{
		async_.set<call_queue, &call_queue::process_call>(this);
		async_.start();
	}

	call_queue(const call_queue&) = delete;
	call_queue& operator = (const call_queue&) = delete;

	void post(const std::function<void ()>& func)
	{
		{
			std::lock_guard<std::mutex> lock(lock_);
			queue_.push_back(func);
		}
		async_.send();
	}

	void post(std::function<void ()>&& func)
	{
		{
			std::lock_guard<std::mutex> lock(lock_);
			queue_.push_back(std::move(func));
		}
		async_.send();
	}

private:
	void process_call(ev::async&, int)
	{
		std::list<std::function<void ()> > queue;

		{
			std::lock_guard<std::mutex> lock(lock_);
			queue.swap(queue_);
		}

		for (auto& i: queue)
		{
			i();
		}
	}

	std::list<std::function<void ()> > queue_;
	std::mutex lock_;
	ev::async async_;
};
