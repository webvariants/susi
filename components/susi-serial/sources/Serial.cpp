#include "susi/Serial.h"

/**
 * Thanks to wallyk for sharing this code sample.
 * http://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c?answertab=votes#tab-top
 **/

void Serial::open() {
	fd = ::open(port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		std::string message = "error opening serial port: ";
		message += strerror(errno);
		throw std::runtime_error{message};
	}

	set_interface_attribs(fd, speed, char_size, parity);
	set_blocking(fd, 0);
}

size_t Serial::read(char* buff, size_t maxlen) {
	size_t byteCount = 0;

	while (1) {
		char temp[maxlen];
		ssize_t length = ::read(fd, temp, sizeof(temp));

		if (length == -1) {
			return 0;
		} else if (length == 0) {
			break;
		} else {
			for (ssize_t i = 0; i < length; i++) {
				buff[byteCount] = temp[i];
				byteCount++;
			}
		}
	}

	buff[byteCount+1] = '\0';
	return byteCount;
}

std::string Serial::readline() {
	char buf[64];

	while (true) {
		int n = read(buf, sizeof buf);
		if (n < 0) {
			throw std::runtime_error{"lost serial connection"};
		}

		lineBuff += std::string(buf, n);
		size_t pos;

		if ((pos = lineBuff.find('\n')) != std::string::npos) {
			std::string line = lineBuff.substr(0, pos);
			lineBuff = lineBuff.substr(pos + 1);
			return line;
		}
	}
}

size_t Serial::write(const char* buff, size_t len){
	return ::write(fd, buff, len);
}

size_t Serial::write(const std::string & data){
	return write((char*)data.c_str(), data.size());
}

int Serial::set_interface_attribs (int fd, int speed, int char_size, int parity) {
	struct termios tty;
	memset(&tty, 0, sizeof tty);

	if (tcgetattr (fd, &tty) != 0) {
		std::cout << "error " << errno << " from tcgetattr" << std::endl;
		return -1;
	}

	cfsetospeed(&tty, speed);
	cfsetispeed(&tty, speed);

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | char_size;   // bit length - 8-bit chars
	// disable IGNBRK for mismatched speed tests; otherwise receive break as \000 chars
	tty.c_iflag &= ~IGNBRK;         // disable break processing
	tty.c_lflag = 0;                // no signaling chars, no echo,

	// no canonical processing
	tty.c_oflag = 0;                // no remapping, no delays
	tty.c_cc[VMIN]  = 0;            // read doesn't block
	tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

	tty.c_iflag &= ~(IXON | IXOFF | IXANY);   // shut off xon/xoff ctrl
	tty.c_cflag |= (CLOCAL | CREAD);          // ignore modem controls,

	// enable reading
	tty.c_cflag &= ~(PARENB | PARODD);        // shut off parity
	tty.c_cflag |= parity;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		std::cout << "error " << errno << " from tcsetattr" << std::endl;
		return -1;
	}
	return 0;
}

void Serial::set_blocking (int fd, int should_block) {
	struct termios tty;
	memset(&tty, 0, sizeof tty);

	if (tcgetattr(fd, &tty) != 0) {
		std::cout << "error " << errno << " from tcgetattr" << std::endl;
		return;
	}

	tty.c_cc[VMIN]  = should_block ? 1 : 0;
	tty.c_cc[VTIME] = 5;   // 0.5 seconds read timeout

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		std::cout << "error " << errno << " setting term attributes" << std::endl;
	}
}
