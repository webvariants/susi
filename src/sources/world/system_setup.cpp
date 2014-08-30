#include "world/ComponentManager.h"
#include "events/Manager.h"
#include "heartbeat/HeartBeat.h"

#include "world/system_setup.h"

std::shared_ptr<Susi::System::ComponentManager> Susi::System::createSusiComponentManager(Susi::Util::Any::Object config){
	using Susi::Util::Any;
	using Susi::System::Component;
	using Susi::System::ComponentManager;
	auto manager = std::make_shared<ComponentManager>(config);
	
	/*
		Register event manager
	 */
	manager->registerComponent("eventmanager",[](ComponentManager *manager, Any conf){
		
		class EventManagerComponent : public Susi::Events::Manager, public Component {
		public:
			EventManagerComponent(size_t workers, size_t queuelen) : Susi::Events::Manager{workers,queuelen} {}
			virtual void start() override {}
			virtual void stop() override {Susi::Events::Manager::stop();}
		};

		size_t workers = 4;
		size_t queuelen = 32;
		if(conf["workers"].isInteger()){
			workers = static_cast<int>(conf["workers"]);
		}
		if(conf["queuelen"].isInteger()){
			queuelen = static_cast<int>(conf["queuelen"]);
		}

		return std::shared_ptr<Component>{new EventManagerComponent{workers,queuelen}};
	});

	/*
		register heartbeat
	 */
	manager->registerDependency("heartbeat","eventmanager");
	manager->registerComponent("heartbeat",[](ComponentManager *manager, Any conf){
		
		class HeartBeatComponent : public Component {
		protected:
			std::shared_ptr<Susi::HeartBeat> heartBeat{nullptr};
		public:
			virtual void start() override {
				heartBeat = std::make_shared<Susi::HeartBeat>();
			}
			virtual void stop() override {
				heartBeat.reset();
			}
		};
		
		return std::shared_ptr<Component>{new HeartBeatComponent{}};
	});


	return manager;
}