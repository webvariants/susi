#include "susi/MongoDBComponent.h"
#include "susi/BaseApp.h"

class MongoDBApp : public Susi::BaseApp {
	protected:
	    std::shared_ptr<Susi::MongoDBComponent> _mongoDBComponent;
	public:
	    MongoDBApp(int argc, char **argv) : Susi::BaseApp{argc,argv} {}
	    virtual ~MongoDBApp() {}
	    virtual void start() override {
	        _mongoDBComponent.reset(new Susi::MongoDBComponent{*_susi,_config["component"]});
	    }
};

int main(int argc, char *argv[]) {
    try {
        MongoDBApp app{argc,argv};
        app.start();
        app.join();
    } catch (const std::exception & e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
