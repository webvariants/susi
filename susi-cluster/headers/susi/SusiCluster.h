#include "susi/SusiClient.h"

class SusiCluster {

public:
    SusiCluster(std::string addr,short port, std::string key, std::string cert);
    void join();

    void registerNode(std::string id, std::string addr,short port, std::string key, std::string cert);
    bool forwardProcessorEvent(std::string topic, std::string id);
    bool forwardConsumerEvent(std::string topic, std::string id);

protected:
    std::shared_ptr<Susi::SusiClient> _susi;
    std::map<std::string,std::shared_ptr<Susi::SusiClient>> _nodes;
};

