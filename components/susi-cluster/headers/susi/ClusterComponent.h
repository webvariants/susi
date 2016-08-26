#include "susi/SusiClient.h"

namespace Susi {
    class ClusterComponent {

    public:
        ClusterComponent(Susi::SusiClient & susi, BSON::Value & config);
        void join();

    protected:
        Susi::SusiClient & _susi;
        BSON::Value _config;
        std::map<std::string,std::shared_ptr<Susi::SusiClient>> _nodes;

        void registerNode(std::string id, std::string addr,short port, std::string key, std::string cert);
        bool forwardProcessorEvent(std::string topic, std::string id);
        bool forwardConsumerEvent(std::string topic, std::string id);
        bool registerProcessor(std::string topic, std::string id);
        bool registerConsumer(std::string topic, std::string id);
    };
}
