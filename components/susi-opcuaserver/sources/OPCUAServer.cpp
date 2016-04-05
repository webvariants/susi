#include "susi/OPCUAServer.h"

Susi::OPCUAServer::OPCUAServer(Susi::SusiClient & susi) :
  susi_{susi} {
    nl = UA_ServerNetworkLayerTCP(UA_ConnectionConfig_standard, 16664);
    config.logger = Logger_Stdout;
    config.networkLayers = &nl;
    config.networkLayersSize = 1;
    server = UA_Server_new(config);
    addInt32Node("the.answer",42);
}

void Susi::OPCUAServer::join(){
  running = true;
  UA_Server_run(server, &running);
  UA_Server_delete(server);
	susi_.join();
}
