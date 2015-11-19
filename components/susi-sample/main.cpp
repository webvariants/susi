#include "susi/SampleComponent.h"
#include "susi/BaseApp.h"

class SampleApp : public Susi::BaseApp {
protected:
    std::shared_ptr<Susi::SampleComponent> _sampleComponent;
public:
    SampleApp(int argc, char **argv) : Susi::BaseApp{argc,argv} {}
    virtual ~SampleApp() {}
    virtual void start() override {
        _sampleComponent.reset(new Susi::SampleComponent{*_susi,_config["component"]});
    }
};

int main(int argc, char *argv[]){
    try{
        SampleApp app{argc,argv};
        app.join();
    }catch(const std::exception & e){
        std::cout << e.what() << std::endl;
    }
    return 0;
}
