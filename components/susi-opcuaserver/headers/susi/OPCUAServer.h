#include "susi/SusiClient.h"
#include "susi/open62541.h"

namespace Susi {
	class OPCUAServer {
	public:
		OPCUAServer(Susi::SusiClient & susi);
		void stop(){

			opcuaServerRunning = false;
		}

	protected:
		Susi::SusiClient & susi;
		std::thread runloop;
		bool opcuaServerRunning;

		static void onRead(void *handle, const UA_NodeId nodeid, const UA_Variant *data, const UA_NumericRange *range) {
			auto self = (Susi::OPCUAServer*)handle;
			std::string node{(char*)nodeid.identifier.string.data,nodeid.identifier.string.length};
			std::cout<<"read request for node " << node << std::endl;
			auto event = self->susi.createEvent("opcua::read");
			event->payload = BSON::Object{
				{"node",node},
			};
			self->susi.publish(std::move(event));

		}

		static void onWrite(void *handle, const UA_NodeId nodeid, const UA_Variant *data, const UA_NumericRange *range) {
			auto self = (Susi::OPCUAServer*)handle;
			UA_Int32 value = -1;
			std::string node{(char*)nodeid.identifier.string.data,nodeid.identifier.string.length};
			if(data->type == &UA_TYPES[UA_TYPES_INT32]) {
				value = *(UA_Int32*)data->data;
				std::cout<<"write request for node " << node <<" : "<<value<< std::endl;
				auto event = self->susi.createEvent("opcua::write");
				event->payload = BSON::Object{
					{"value",(BSON::int64)value},
					{"node",node}
				};
				self->susi.publish(std::move(event));
			}
		}

		void addInt32Node(UA_Server *server, const char * id, int32_t initial = 0){
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
