#include "susi/SusiClient.h"

namespace Susi {
    class ShellComponent {

    public:
        ShellComponent(Susi::SusiClient & susi, BSON::Value & config);
        void join();

        void handleExecEvent(Susi::EventPtr event);
        void handleExec(std::string command,
                        std::string & stdin,
                        std::string & stdout,
                        std::string & stderr,
                        int & status);

    protected:
        Susi::SusiClient & _susi;
        BSON::Object _commands;
        std::map<std::string,std::shared_ptr<Susi::SusiClient>> _nodes;
    };
}
