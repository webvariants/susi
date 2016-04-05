#include "susi/SusiClient.h"
#include "susi/open62541.h"

namespace Susi {
	class OPCUAServer {
	public:
		OPCUAServer(Susi::SusiClient & susi);
		void join();

	protected:
		Susi::SusiClient & susi_;
		boost::asio::io_service io_service_;
		UA_ServerConfig config = UA_ServerConfig_standard;
		UA_ServerNetworkLayer nl;
		UA_Server *server;
		bool running = true;
		std::thread runloop;

		static void onRead(void *handle, const UA_NodeId nodeid, const UA_Variant *data, const UA_NumericRange *range) {
	  	std::cout << "onRead; handle is:" <<  (uintptr_t)handle << std::endl;;
		}

		static void onWrite(void *handle, const UA_NodeId nodeid, const UA_Variant *data, const UA_NumericRange *range) {
	  	std::cout << "onWrite; handle is:" << (uintptr_t)handle << std::endl;;
		}

		void addInt32Node(const char * id, int32_t initial = 0){
		  UA_VariableAttributes attr;
		  UA_VariableAttributes_init(&attr);
		  UA_Int32 myInteger = initial;
		  UA_Variant_setScalar(&attr.value, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
		  attr.description = UA_LOCALIZEDTEXT("en_US",(char*)id);
		  attr.displayName = UA_LOCALIZEDTEXT("en_US",(char*)id);
		  UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, (char*)id);
		  UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, (char*)id);
		  UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
		  UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
		  UA_Server_addVariableNode(server, myIntegerNodeId, parentNodeId,
		                            parentReferenceNodeId, myIntegerName,
		                            UA_NODEID_NULL, attr, NULL, NULL);
		  UA_ValueCallback callback = {(void*)this, &OPCUAServer::onRead, &OPCUAServer::onWrite};
		  UA_Server_setVariableNode_valueCallback(server, myIntegerNodeId, callback);
		}
	};
}
