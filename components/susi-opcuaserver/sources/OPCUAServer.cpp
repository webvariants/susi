#include "susi/OPCUAServer.h"

Susi::OPCUAServer::OPCUAServer(Susi::SusiClient & susi) :
  susi{susi} {
    runloop = std::move(std::thread{[this](){
      UA_ServerConfig config = UA_ServerConfig_standard;
      UA_ServerNetworkLayer nl = UA_ServerNetworkLayerTCP(UA_ConnectionConfig_standard, 16664);
      //config.logger = Logger_Stdout;
      config.networkLayers = &nl;
      config.networkLayersSize = 1;
      UA_Server *server = UA_Server_new(config);

      addInt32Node(server,"the.answer",42);

      opcuaServerRunning = true;
      UA_Server_run(server, &opcuaServerRunning);
      UA_Server_delete(server);
    }});
}
