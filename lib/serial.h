#pragma once

#include <chrono>
#include <boost/intrusive_ptr.hpp>

class serial
{
public:
	using scoped_lock = boost::intrusive_ptr<serial>;

	serial() = default;
	~serial()
	{
		::close(fd_);
	}

	void open(const char* pathname, int flags);
	ssize_t read(void* buffer, size_t count) const;
	ssize_t write(const void* buffer, size_t count) const;

	int fd() const { return fd_; }
	explicit operator bool() { return fd_ != -1; }

protected:
	serial(const serial&) = delete;
	serial& operator=(const serial&) = delete;

	bool poll(short events, std::chrono::milliseconds& timeout) const;

	std::chrono::milliseconds timeout_{100};
	int fd_ = -1;
	int lock_count_ = 0;

	friend void intrusive_ptr_add_ref(serial* s);
	friend void intrusive_ptr_release(serial* s);
};

