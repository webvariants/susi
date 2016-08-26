#include "susi/Authenticator.h"
#include "susi/BaseApp.h"

class AuthenticatorApp : public Susi::BaseApp {
protected:
    std::shared_ptr<Susi::Authenticator> _authenticator;
public:
    AuthenticatorApp(int argc, char **argv) : Susi::BaseApp{argc,argv} {}
    virtual ~AuthenticatorApp() {}
    virtual void start() override {
        _authenticator.reset(new Susi::Authenticator{*_susi,_config["component"]});
    }
};

int main(int argc, char *argv[]){
    try{
        AuthenticatorApp app{argc,argv};
		app.start();
        app.join();
    }catch(const std::exception & e){
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
