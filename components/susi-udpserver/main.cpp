#include "susi/UDPServerComponent.h"
#include "susi/BaseApp.h"

class UDPApp : public Susi::BaseApp {
protected:
    std::shared_ptr<Susi::UDPServerComponent> _udpComponent;
public:
    UDPApp(int argc, char **argv) : Susi::BaseApp{argc,argv} {}
    virtual ~UDPApp() {}
    virtual void start() override {
        _udpComponent.reset(new Susi::UDPServerComponent{*_susi,_config["component"]});
    }
};

int main(int argc, char *argv[]){
    try{
        UDPApp app{argc,argv};
        app.start();
        app.join();
    }catch(const std::exception & e){
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
