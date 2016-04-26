#include "susi/SusiClient.h"
#include "Serial.h"
#include <iostream>

namespace Susi {

    class SerialComponent {

    public:
        SerialComponent(Susi::SusiClient & susi, BSON::Value & config);
		~SerialComponent();
		void join();

    protected:
		void initPorts();
		void initPort(const std::string & id, const std::string & portname, const int & baudrate, const int & char_size, const int & parity);
        Susi::SusiClient & _susi;
        BSON::Value & _config;
		std::map<std::string, std::shared_ptr<Serial>> ports;
		std::map<std::string, std::thread> threads;
		std::atomic<bool> running;
    };

}
