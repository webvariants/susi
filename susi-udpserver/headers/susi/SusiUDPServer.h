#include "susi/SusiClient.h"

namespace Susi {
	
	class SusiUDPServer {

	public:
		SusiUDPServer(std::string addr,short port, std::string key, std::string cert, unsigned short udpPort);

		void stop();
		
		void join();
		
		~SusiUDPServer();

	protected:
		std::shared_ptr<Susi::SusiClient> _susi;
		boost::asio::io_service _io_service;
		boost::asio::ip::udp::socket _socket; 
		std::thread _runloop;
		char _packet[1<<16];
		boost::asio::ip::udp::endpoint _sender_endpoint;

		void do_receive();

		void do_send(const std::string & message, boost::asio::ip::udp::endpoint receiver);
	};
}


