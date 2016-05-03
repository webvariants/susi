#include "susi/SerialComponent.h"

Susi::SerialComponent::SerialComponent(Susi::SusiClient & susi, BSON::Value & config) : _susi{susi}, _config{config} {

	running.store(true);
	initPorts();

    _susi.registerProcessor("serial::write", [this](Susi::EventPtr event) {
        auto & payload  = event->payload;
        std::string id  = payload["id"];
        std::string msg = payload["msg"];
		ports[id]->write(msg);
    });
}

void Susi::SerialComponent::initPorts() {
    auto ports = _config["ports"].getArray();

    for (auto & port : ports) {
        auto id        = port["id"].getString();
        auto portname  = port["port"].getString();
        auto baudrate  = port["baudrate"].getInt64();
		auto char_size = port["char_size"].getString();
		auto parity    = port["parity"].getString();

		auto converted_baudrate  = B115200;
		auto converted_char_size = CS8;
		auto converted_parity    = Serial::ODD;

		if (baudrate == 0) {
			converted_baudrate = B0;
		} else if (baudrate == 50) {
			converted_baudrate = B50;
		} else if (baudrate == 75) {
			converted_baudrate = B75;
		} else if (baudrate == 110) {
			converted_baudrate = B110;
		} else if (baudrate == 134) {
			converted_baudrate = B134;
		} else if (baudrate == 150) {
			converted_baudrate = B150;
		} else if (baudrate == 200) {
			converted_baudrate = B200;
		} else if (baudrate == 300) {
			converted_baudrate = B300;
		} else if (baudrate == 600) {
			converted_baudrate = B600;
		} else if (baudrate == 1200) {
			converted_baudrate = B1200;
		} else if (baudrate == 1800) {
			converted_baudrate = B1800;
		} else if (baudrate == 2400) {
			converted_baudrate = B2400;
		} else if (baudrate == 4800) {
			converted_baudrate = B4800;
		} else if (baudrate == 9600) {
			converted_baudrate = B9600;
		} else if (baudrate == 19200) {
			converted_baudrate = B19200;
		} else if (baudrate == 38400) {
			converted_baudrate = B38400;
		} else if (baudrate == 57600) {
			converted_baudrate = B57600;
		} else if (baudrate == 115200) {
			converted_baudrate = B115200;
		} else if (baudrate == 230400) {
			converted_baudrate = B230400;
		}

		if (char_size == "CS8" || char_size == "cs8") {
			converted_char_size = CS8;
		} else if (char_size == "CS7" || char_size == "cs7") {
			converted_char_size = CS7;
		} else if (char_size == "CS6" || char_size == "cs6") {
			converted_char_size = CS6;
		} else if (char_size == "CS5" || char_size == "cs5") {
			converted_char_size = CS5;
		}

		if (parity == "ODD" || parity == "odd") {
			converted_parity = Serial::ODD;
		} else if (parity == "EVEN" || parity == "even") {
			converted_parity = Serial::EVEN;
		} else if (parity == "NONE" || parity == "none") {
			converted_parity = Serial::NONE;
		}

        initPort(id, portname, converted_baudrate, converted_char_size, converted_parity);
    }
}

void Susi::SerialComponent::initPort(const std::string & id, const std::string & portname, const int & baudrate, const int & char_size, const int & parity) {
	auto port = std::make_shared<Serial>(portname, baudrate, char_size, parity);
	try {
		port->open();
		ports[id] = port;

		auto t = std::thread{[this,port,id]() {
			while (running.load()) {
				char data[4096];
				size_t len = port->read(data, sizeof(data));

				if (len > 0) {
					std::string str{data,len};
					auto event = _susi.createEvent("serial::data");
					event->payload = BSON::Object{
						{"id",id},
						{"data",str}
					};
					_susi.publish(std::move(event));
				}
			}
		}};
		threads[id] = std::move(t);

		std::cout << "serial port " << id << " is open" << std::endl;
	} catch (const std::exception & e) {
		std::cout << "failed to open serial connection to " << id << std::endl;
	}
}

void Susi::SerialComponent::join() {
	_susi.join();
}

Susi::SerialComponent::~SerialComponent() {
	running.store(false);
	for (auto & kv : threads) {
		if (kv.second.joinable()) {
			kv.second.join();
		}
	}
}
