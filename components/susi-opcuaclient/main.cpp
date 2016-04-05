#include "susi/OPCUAClient.h"
#include "susi/BaseApp.h"

class OPCUAClientApp : public Susi::BaseApp {
protected:
    std::shared_ptr<Susi::OPCUAClient> _opcuaClientComponent;
public:
    OPCUAClientApp(int argc, char **argv) : Susi::BaseApp{argc,argv} {}
    virtual ~OPCUAClientApp() {}
    virtual void start() override {
        _opcuaClientComponent.reset(new Susi::OPCUAClient{*_susi});
    }
};

int main(int argc, char *argv[]){
    try{
        OPCUAClientApp app{argc,argv};
        app.start();
        app.join();
    }catch(const std::exception & e){
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
