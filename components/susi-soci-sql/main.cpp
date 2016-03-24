#include "susi/SociSQLComponent.h"
#include "susi/BaseApp.h"

class SociSQLApp : public Susi::BaseApp {
protected:
    std::shared_ptr<Susi::SociSQLComponent> _sociSQLComponent;
public:
    SociSQLApp(int argc, char **argv) : Susi::BaseApp{argc,argv} {}
    virtual ~SociSQLApp() {}
    virtual void start() override {
        _sociSQLComponent.reset(new Susi::SociSQLComponent{*_susi,_config["component"]});
    }
};

int main(int argc, char *argv[]){
    try{
        SociSQLApp app{argc,argv};
        app.start();
        app.join();
    }catch(const std::exception & e){
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
