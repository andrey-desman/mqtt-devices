#pragma once

#include <cinttypes>
#include <unistd.h>
#include <chrono>
#include <unistd.h>

class serial
{
public:
	class scoped_lock
	{
	public:
		scoped_lock(serial* s) : device_(s) { device_->lock(); }
		~scoped_lock() { device_->unlock(); }
	private:
		serial* device_;
	};

	serial() = default;
	~serial()
	{
		::close(fd_);
	}

	void open(const char* pathname, int flags);
	ssize_t read(void* buffer, size_t count) const;
	bool read_exact(void* buffer, size_t count) const;
	ssize_t write(const void* buffer, size_t count) const;

	int fd() const { return fd_; }
	explicit operator bool() { return fd_ != -1; }

protected:
	serial(const serial&) = delete;
	serial& operator=(const serial&) = delete;

	void lock();
	void unlock();

	bool poll(short events, std::chrono::milliseconds& timeout) const;

	std::chrono::milliseconds timeout_{100};
	int fd_ = -1;
	int lock_count_ = 0;
};

