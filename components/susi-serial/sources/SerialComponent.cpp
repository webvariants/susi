#include "susi/SerialComponent.h"

Susi::SerialComponent::SerialComponent(Susi::SusiClient & susi, BSON::Value & config) : _susi{susi}, _config{config} {

	running.store(true);

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
		auto char_size = port["char_size"].getInt64();
		auto parity    = port["parity"].getString();

		auto converted_baudrate  = B115200;
		auto converted_char_size = CS8;
		auto converted_parity    = Serial::ODD;

		switch (baudrate) {
			case 0:      converted_baudrate = B0;      break;
			case 50:     converted_baudrate = B50;     break;
			case 75:     converted_baudrate = B75;     break;
			case 110:    converted_baudrate = B110;    break;
			case 134:    converted_baudrate = B134;    break;
			case 150:    converted_baudrate = B150;    break;
			case 200:    converted_baudrate = B200;    break;
			case 300:    converted_baudrate = B300;    break;
			case 600:    converted_baudrate = B600;    break;
			case 1200:   converted_baudrate = B1200;   break;
			case 1800:   converted_baudrate = B1800;   break;
			case 2400:   converted_baudrate = B2400;   break;
			case 4800:   converted_baudrate = B4800;   break;
			case 9600:   converted_baudrate = B9600;   break;
			case 19200:  converted_baudrate = B19200;  break;
			case 38400:  converted_baudrate = B38400;  break;
			case 57600:  converted_baudrate = B57600;  break;
			case 115200: converted_baudrate = B115200; break;
			case 230400: converted_baudrate = B230400; break;
			default: throw std::runtime_error{"Unknown baudrate"};
		}

		switch (char_size) {
			case 8: converted_char_size = CS8; break;
			case 7: converted_char_size = CS7; break;
			case 6: converted_char_size = CS6; break;
			case 5: converted_char_size = CS5; break;
			default: throw std::runtime_error{"Unknown char size"};
		}

		if (parity == "ODD" || parity == "odd") {
			converted_parity = Serial::ODD;
		} else if (parity == "EVEN" || parity == "even") {
			converted_parity = Serial::EVEN;
		} else if (parity == "NONE" || parity == "none") {
			converted_parity = Serial::NONE;
		} else {
			throw std::runtime_error{"Unknown parity"};
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

		std::cout << "serial port " << id << " is now open" << std::endl;
	} catch (const std::exception & e) {
		std::cout << "failed to open serial connection to " << id << " (" << e.what() << ")" << std::endl;
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
