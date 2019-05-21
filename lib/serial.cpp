#include "log.h"
#include "util.h"
#include "serial.h"

#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>

namespace
{
	inline bool would_block()
	{
		return errno == EAGAIN || errno == EWOULDBLOCK;
	}
}

void serial::open(const char* pathname, int flags)
{
	int fd;
	check(fd = ::open(pathname, flags | O_NONBLOCK), "failed to open tty device");
	fd_guard f(&fd);

	termios tty;

	check(tcgetattr(fd, &tty), "Failed to get serial attrs");

	cfsetospeed(&tty, B9600);
	cfsetispeed(&tty, B9600);

	cfmakeraw(&tty);

	tty.c_cc[VMIN] = 0;
	tty.c_cc[VTIME] = 0;

	check(tcsetattr(fd, TCSANOW, &tty), "Failed to set tty attributes");
	tcflush(fd, TCIFLUSH);

	close(fd_);
	fd_ = *f.release();
}

ssize_t serial::read(void* buffer, size_t count) const
{
	check(fd_, "device is not opened");
	ssize_t r = ::read(fd_, buffer, count);
	if (r > 0)
		return r;
	std::chrono::milliseconds timeout(timeout_);
	if (poll(POLLIN, timeout))
		return ::read(fd_, buffer, count);
	return 0;
}

bool serial::read_exact(void* buffer, size_t count) const
{
	check(fd_, "device is not opened");
	std::chrono::milliseconds timeout(timeout_);
	while (count)
	{
		int bytes_read = ::read(fd_, buffer, count);

		if (bytes_read == -1 && !would_block())
		{
			int err = errno;
			LOG(error, "failed to read %d bytes from fd %d, error %d: %s", int(count), fd_,
				err, strerror(err));
			return false;
		}

		if (bytes_read > 0)
			count -= bytes_read;
		else if (!poll(POLLIN, timeout))
			return false;
	}
	return true;
}

ssize_t serial::write(const void* buffer, size_t count) const
{
	check(fd_, "device is not opened");

	std::chrono::milliseconds timeout(timeout_);
	const char* ptr = reinterpret_cast<const char*>(buffer);
	ssize_t bytes_written = 0;

	while (bytes_written < count)
	{
		ssize_t r = ::write(fd_, ptr, count - bytes_written);
		if (r == -1)
		{
			if (would_block() && poll(POLLOUT, timeout))
				continue;
			break;
		}
		bytes_written += r;
		ptr += r;
	}
	return bytes_written;
}

bool serial::poll(short event, std::chrono::milliseconds& timeout) const
{
	check(fd_, "device is not opened");

	if (timeout.count() <= 0)
		return false;
	pollfd p;
	p.fd = fd_;
	p.events = event;

	using namespace std::chrono;
	time_point<steady_clock> n = steady_clock::now();
	if (::poll(&p, 1, timeout.count()) == -1)
		return false;
	timeout -= duration_cast<milliseconds>(steady_clock::now() - n);
	return p.revents & event;
}

void serial::lock()
{
	check(fd_, "device is not opened");
	if (lock_count_++ == 0)
	{
		LOG(debug, "lock %d", fd_);
		lockf(fd_, F_LOCK, 0);
	}
}

void serial::unlock()
{
	check(fd_, "device is not opened");
	if (--lock_count_ == 0)
	{
		LOG(debug, "unlock %d", fd_);
		lockf(fd_, F_ULOCK, 0);
	}
}

