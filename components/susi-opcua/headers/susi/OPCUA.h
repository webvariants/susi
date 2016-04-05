#include "susi/SusiClient.h"
#include "susi/open62541.h"

namespace Susi {
	class OPCUA {
	public:
		OPCUA(Susi::SusiClient & susi);
		void join();

	protected:
		Susi::SusiClient & susi_;
		boost::asio::io_service io_service_;

		std::shared_ptr<UA_Client> createClient(std::string endpoint){
			auto deleter = [](UA_Client *ptr){UA_Client_delete(ptr);};
			auto client = std::shared_ptr<UA_Client>{
				UA_Client_new(UA_ClientConfig_standard, Logger_Stdout),
				deleter
			};
			UA_StatusCode retval = UA_Client_connect(client.get(), UA_ClientConnectionTCP,endpoint.c_str());
			if(retval != UA_STATUSCODE_GOOD) {
				throw std::runtime_error{"can not connect to opcua endpoint"};
			}
			return client;
		}

		UA_Int32 getInt32(std::shared_ptr<UA_Client> client, std::string id){
		  UA_Int32 value = 0;
		  UA_ReadRequest rReq;
		  UA_ReadRequest_init(&rReq);
		  rReq.nodesToRead =  (UA_ReadValueId*)UA_Array_new(1, &UA_TYPES[UA_TYPES_READVALUEID]);
		  rReq.nodesToReadSize = 1;
		  rReq.nodesToRead[0].nodeId = UA_NODEID_STRING_ALLOC(1, (char*)id.c_str()); /* assume this node exists */
		  rReq.nodesToRead[0].attributeId = UA_ATTRIBUTEID_VALUE;

		  UA_ReadResponse rResp = UA_Client_Service_read(client.get(), rReq);
		  if(rResp.responseHeader.serviceResult == UA_STATUSCODE_GOOD &&
		     rResp.resultsSize > 0 && rResp.results[0].hasValue &&
		     UA_Variant_isScalar(&rResp.results[0].value) &&
		     rResp.results[0].value.type == &UA_TYPES[UA_TYPES_INT32]) {
		      value = *(UA_Int32*)rResp.results[0].value.data;
		  }
		  UA_ReadRequest_deleteMembers(&rReq);
		  UA_ReadResponse_deleteMembers(&rResp);
		  return value;
		}

		void setInt32(std::shared_ptr<UA_Client> client, std::string id, UA_Int32 value){
		  UA_Variant *myVariant = UA_Variant_new();
		  UA_Variant_setScalarCopy(myVariant, &value, &UA_TYPES[UA_TYPES_INT32]);
		  UA_Client_writeValueAttribute(client.get(), UA_NODEID_STRING(1, (char*)id.c_str()), myVariant);
		  UA_Variant_delete(myVariant);
		}

	};
}
