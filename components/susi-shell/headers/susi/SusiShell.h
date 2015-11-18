#include "susi/SusiClient.h"

class SusiShell {

public:
    SusiShell(std::string addr,short port, std::string key, std::string cert, BSON::Object commands);
    void join();

    void handleExecEvent(Susi::EventPtr event);
    void handleExec(std::string command,
                    std::string & stdin,
                    std::string & stdout,
                    std::string & stderr);

protected:
    std::shared_ptr<Susi::SusiClient> _susi;
    BSON::Object _commands;
    std::map<std::string,std::shared_ptr<Susi::SusiClient>> _nodes;
};
