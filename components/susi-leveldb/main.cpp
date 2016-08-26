#include "susi/LevelDBComponent.h"
#include "susi/BaseApp.h"

class LevelDBApp : public Susi::BaseApp {
protected:
    std::shared_ptr<Susi::LevelDBComponent> _leveldbComponent;
public:
    LevelDBApp(int argc, char **argv) : Susi::BaseApp{argc,argv} {}
    virtual ~LevelDBApp() {}
    virtual void start() override {
        _leveldbComponent.reset(new Susi::LevelDBComponent{*_susi,_config["component"]["db"]});
    }
};

int main(int argc, char *argv[]){
    try{
        LevelDBApp app{argc,argv};
        app.start();
        app.join();
    }catch(const std::exception & e){
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
