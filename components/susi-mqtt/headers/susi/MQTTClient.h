#include "leveldb/db.h"
#include "susi/SusiClient.h"
#include <mosquittopp.h>

namespace Susi {
	class MQTTClient : public mosqpp::mosquittopp {

	public:
		MQTTClient(Susi::SusiClient & susi, BSON::Value & config);
		void join();

	protected:
		Susi::SusiClient & susi_;
		std::thread runloop_;
		std::vector<std::string> subscriptions_;
		
		void subscribe(const std::string & topic);
		void forward(const std::string & topic);

		void on_connect(int rc);
		void on_message(const struct mosquitto_message *message);
		void on_subscribe(int mid, int qos_count, const int *granted_qos);
	};
}
