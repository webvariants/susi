#include "susi/SusiClient.h"

namespace Susi {

	class UDPServerComponent {

	public:
		UDPServerComponent(Susi::SusiClient & susi, BSON::Value & config);

		void stop();

		void join();

		~UDPServerComponent();

	protected:
		Susi::SusiClient & _susi;
		boost::asio::io_service _io_service;
		boost::asio::ip::udp::socket _socket;
		std::thread _runloop;
		char _packet[1<<16];
		boost::asio::ip::udp::endpoint _sender_endpoint;

		// map from address -> topic -> registration-id
		std::map< boost::asio::ip::udp::endpoint, std::map< std::string, std::uint64_t >> _registrations;

		void do_receive();

		void do_send(const std::string & message, boost::asio::ip::udp::endpoint receiver);

		void handlePublish(BSON::Value & event, boost::asio::ip::udp::endpoint & sender);
		void handleRegister(BSON::Value & event, boost::asio::ip::udp::endpoint & sender);
		void handleUnregister(BSON::Value & event, boost::asio::ip::udp::endpoint & sender);

	};
}
