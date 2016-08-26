#include "susi/StateFileComponent.h"
#include "susi/BaseApp.h"

class StateFileApp : public Susi::BaseApp {
protected:
    std::shared_ptr<Susi::StateFileComponent> _stateFileComponent;
public:
    StateFileApp(int argc, char **argv) : Susi::BaseApp{argc,argv} {}
    virtual ~StateFileApp() {}
    virtual void start() override {
        _stateFileComponent.reset(new Susi::StateFileComponent{*_susi,_config["component"]});
    }
};

int main(int argc, char *argv[]){
    try{
        StateFileApp app{argc,argv};
        app.start();
        app.join();
    }catch(const std::exception & e){
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
