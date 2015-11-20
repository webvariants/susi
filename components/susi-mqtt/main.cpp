#include "susi/MQTTClient.h"
#include "susi/BaseApp.h"

class MQTTApp : public Susi::BaseApp {
protected:
    std::shared_ptr<Susi::MQTTClient> _mqttComponent;
public:
    MQTTApp(int argc, char **argv) : Susi::BaseApp{argc,argv} {}
    virtual ~MQTTApp() {}
    virtual void start() override {
        _mqttComponent.reset(new Susi::MQTTClient{*_susi,_config["component"]});
    }
};

int main(int argc, char *argv[]){
    try{
        MQTTApp app{argc,argv};
        app.start();
        app.join();
    }catch(const std::exception & e){
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
