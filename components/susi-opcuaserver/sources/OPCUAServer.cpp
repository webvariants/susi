#include "susi/OPCUAServer.h"
#include <boost/asio.hpp>
using boost::asio::ip::tcp;

Susi::OPCUAServer::OPCUAServer(Susi::SusiClient & susi) :
  susi_{susi} {
    UA_ServerConfig config = UA_ServerConfig_standard;
    UA_ServerNetworkLayer nl = UA_ServerNetworkLayerTCP(UA_ConnectionConfig_standard, 16664);
    config.logger = Logger_Stdout;
    config.networkLayers = &nl;
    config.networkLayersSize = 1;
    UA_Server *server = UA_Server_new(config);
    addInt32Node("the.answer",42);
}

void Susi::OPCUAServer::join(){
  running = true;
  UA_Server_run(server, &running);
  UA_Server_delete(server);
	susi_.join();
}
