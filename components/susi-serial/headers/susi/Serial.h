#ifndef __SERIAL__
#define __SERIAL__

/**
* Serial Communication class
* ==========================
*
* Public Constructor:
*
* Valid arguments for serial communication are listed below.
*
*
* port:
*  example:  "/dev/ttyUSB0"
*
* speed:
*  B0
*  B50
*  B75
*  B110
*  B134
*  B150
*  B200
*  B300
*  B600
*  B1200
*  B1800
*  B2400
*  B4800
*  B9600
*  B19200
*  B38400
*  B57600
*  B115200
*  B230400
*
* char_size:
*  CS8
*  CS7
*  CS6
*  CS5
*
* paritiy:
*  Serial::NONE
*  Serial::ODD
*  Serial::EVEN
*
*
* Code example:
*
*   Serial serial("/dev/ttyUSB0", B38400, CS8, Serial::ODD);
*   try {
*       serial.open();
*       while (true) {
*           char data[4096];
*           size_t len = serial->read(data, sizeof(data));
*           if (len > 0) {
*               std::string str{data,len};
*               std::cout << "Read: " << str << std::endl;
*           }
*       }
*   }
*   catch (const std::exception & e) {
*       std::cout << "Failed to open serial port" << std::endl;
*   }
*
*/

#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>

#include <cstdlib>
#include <cstring>
#include <iostream>

class Serial {
	public:
		Serial(const std::string & port, int speed, int char_size, int parity) :
				port{port}, speed{speed}, char_size{char_size}, parity{parity} {}

		Serial(const std::string & port) :
				port{port}, speed{B9600}, char_size{CS8}, parity{Serial::NONE} {}

		enum {
			NONE = 0,
			ODD  = PARENB | PARODD,
			EVEN = PARENB
		};

		void open();
		size_t read(char* buff, size_t maxlen);
		std::string readline();
		size_t write(const char* buff, size_t len);
		size_t write(const std::string & data);

	protected:
		std::string port;
		int speed;
		int char_size;
		int parity;
		int fd;
		std::string lineBuff;

		int set_interface_attribs(int fd, int speed, int char_size, int parity);
		void set_blocking(int fd, int should_block);
};

#endif // __SERIAL__
