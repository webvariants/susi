#include "susi/OPCUA.h"
#include "susi/BaseApp.h"

class OPCUAApp : public Susi::BaseApp {
protected:
    std::shared_ptr<Susi::OPCUA> _webhookComponent;
public:
    OPCUAApp(int argc, char **argv) : Susi::BaseApp{argc,argv} {}
    virtual ~OPCUAApp() {}
    virtual void start() override {
        _webhookComponent.reset(new Susi::OPCUA{*_susi});
    }
};

int main(int argc, char *argv[]){
    try{
        OPCUAApp app{argc,argv};
        app.start();
        app.join();
    }catch(const std::exception & e){
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
