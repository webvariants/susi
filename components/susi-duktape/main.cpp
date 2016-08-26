#include "susi/DuktapeEngine.h"
#include "susi/BaseApp.h"

class JSEngineApp : public Susi::BaseApp {
protected:
    std::shared_ptr<Susi::Duktape::JSEngine> _duktapeEngine;
public:
    JSEngineApp(int argc, char **argv) : Susi::BaseApp{argc,argv} {}
    virtual ~JSEngineApp() {}
    virtual void start() override {
        _duktapeEngine.reset(new Susi::Duktape::JSEngine{*_susi,_config["component"]["src"]});
        _duktapeEngine->start();
    }
};

int main(int argc, char *argv[]){
    try{
        JSEngineApp app{argc,argv};
        app.start();
        app.join();
    }catch(const std::exception & e){
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
