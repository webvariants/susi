#include "susi/HeartBeat.h"
#include "susi/BaseApp.h"

class HeartbeatApp : public Susi::BaseApp {
protected:
    std::shared_ptr<Susi::HeartBeat> _heartbeatComponent;
public:
    HeartbeatApp(int argc, char **argv) : Susi::BaseApp{argc,argv} {}
    virtual ~HeartbeatApp() {}
    virtual void start() override {
        _heartbeatComponent.reset(new Susi::HeartBeat{*_susi});
    }
};

int main(int argc, char *argv[]){
    try{
        HeartbeatApp app{argc,argv};
        app.start();
        app.join();
    }catch(const std::exception & e){
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
