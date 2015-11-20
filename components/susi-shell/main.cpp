#include "susi/ShellComponent.h"
#include "susi/BaseApp.h"

class ShellApp : public Susi::BaseApp {
protected:
    std::shared_ptr<Susi::ShellComponent> _shellComponent;
public:
    ShellApp(int argc, char **argv) : Susi::BaseApp{argc,argv} {}
    virtual ~ShellApp() {}
    virtual void start() override {
        _shellComponent.reset(new Susi::ShellComponent{*_susi,_config["component"]});
    }
};

int main(int argc, char *argv[]){
    try{
        ShellApp app{argc,argv};
        app.start();
        app.join();
    }catch(const std::exception & e){
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
