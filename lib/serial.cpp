#include "log.h"
#include "util.h"

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

namespace serial
{

int open(const char* pathname, int flags)
{
	int fd;
	check(fd = ::open(pathname, flags), "failed to open tty device");
	fd_guard f(&fd);

	termios tty{};

	/* Error Handling */

	check(tcgetattr(fd, &tty), "Failed to get serial attrs");

	/* Set Baud Rate */
	cfsetospeed(&tty, B9600);
	cfsetispeed(&tty, B9600);

	tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_oflag &= ~OPOST;
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_cflag &= ~(CSIZE | PARENB);
    tty.c_cflag |= CS8;

	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 0;

	tcflush(fd, TCIFLUSH);

	check(tcsetattr(fd, TCSANOW, &tty), "Failed to set tty attributes");
	return *f.release();
}

}
