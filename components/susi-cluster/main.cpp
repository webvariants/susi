#include "susi/ClusterComponent.h"
#include "susi/BaseApp.h"

class ClusterApp : public Susi::BaseApp {
protected:
    std::shared_ptr<Susi::ClusterComponent> _clusterComponent;
public:
    ClusterApp(int argc, char **argv) : Susi::BaseApp{argc,argv} {}
    virtual ~ClusterApp() {}
    virtual void start() override {
        _clusterComponent.reset(new Susi::ClusterComponent{*_susi,_config["component"]});
    }
};

int main(int argc, char *argv[]){
    try{
        ClusterApp app{argc,argv};
		app.start();
        app.join();
    }catch(const std::exception & e){
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
