#include "susi/OPCUAServer.h"
#include "susi/BaseApp.h"

class OPCUAServerApp : public Susi::BaseApp {
protected:
    std::shared_ptr<Susi::OPCUAServer> _opcuaServerComponent;
public:
    OPCUAServerApp(int argc, char **argv) : Susi::BaseApp{argc,argv} {}
    virtual ~OPCUAServerApp() {}
    virtual void start() override {
        _opcuaServerComponent.reset(new Susi::OPCUAServer{*_susi});
    }
};

int main(int argc, char *argv[]){
    try{
        OPCUAServerApp app{argc,argv};
        app.start();
        app.join();
    }catch(const std::exception & e){
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
