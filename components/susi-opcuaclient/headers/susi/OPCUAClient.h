#include "susi/SusiClient.h"
#include "ua_types.h"
#include "ua_client_highlevel.h"
#include "logger_stdout.h"
#include "networklayer_tcp.h"

namespace Susi {
	class OPCUAClient {
	public:
		OPCUAClient(Susi::SusiClient & susi);
		void join();

	protected:
		Susi::SusiClient & susi;

		struct Subscription {
			std::shared_ptr<UA_Client> client;
			std::string topic;
			UA_UInt32 subId;
			UA_UInt32 monId;
		};
		std::map<UA_UInt32,Subscription> subscriptions; //monId -> sub

		std::shared_ptr<UA_Client> createClient(std::string endpoint);
		UA_Int32 getInt32(std::shared_ptr<UA_Client> client, std::string id);
		void setInt32(std::shared_ptr<UA_Client> client, std::string id, UA_Int32 value);
		void subscribe(std::string endpoint, std::string id, std::string topic);
		static void onChange(UA_UInt32 monId, UA_DataValue *value, void *context);

	};
}
