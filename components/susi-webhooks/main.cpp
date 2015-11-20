#include "susi/Webhooks.h"
#include "susi/BaseApp.h"

class WebhookApp : public Susi::BaseApp {
protected:
    std::shared_ptr<Susi::Webhooks> _webhookComponent;
public:
    WebhookApp(int argc, char **argv) : Susi::BaseApp{argc,argv} {}
    virtual ~WebhookApp() {}
    virtual void start() override {
        _webhookComponent.reset(new Susi::Webhooks{*_susi});
    }
};

int main(int argc, char *argv[]){
    try{
        WebhookApp app{argc,argv};
        app.start();
        app.join();
    }catch(const std::exception & e){
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
