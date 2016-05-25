#include "susi/SerialComponent.h"
#include "susi/BaseApp.h"

class SerialApp : public Susi::BaseApp {
	protected:
	    std::shared_ptr<Susi::SerialComponent> _serialComponent;
	public:
	    SerialApp(int argc, char **argv) : Susi::BaseApp{argc,argv} {}
	    virtual ~SerialApp() {}
	    virtual void start() override {
	        _serialComponent.reset(new Susi::SerialComponent{*_susi,_config["component"]});
			_serialComponent->start();
	    }
};

int main(int argc, char *argv[]) {
    try {
        SerialApp app{argc,argv};
        app.start();
        app.join();
    } catch (const std::exception & e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
