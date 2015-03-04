#include "gtest/gtest.h"
#include "susi/world/ComponentManager.h"

class ComponentManagerTest : public ::testing::Test {
	protected:
		class SampleComponent : public Susi::System::Component {
		public:
			SampleComponent(Susi::Util::Any config){
				if(config.isString()){
					EXPECT_EQ("foobar",static_cast<std::string>(config));
				}
			}
			virtual void start() override {}
			virtual void stop() override {}
		};
};

TEST_F(ComponentManagerTest,Basic) {
	using Susi::Util::Any;
	using namespace Susi::System;
	Any::Object config{{"sample","foobar"}};
	ComponentManager manager{config};
	manager.registerComponent("sample",[](ComponentManager * mgr, Any config){
		return std::shared_ptr<Component>{new SampleComponent{config}};
	});
	EXPECT_TRUE(manager.loadComponent("sample"));
	EXPECT_FALSE(manager.loadComponent("sample"));
	EXPECT_TRUE(manager.startComponent("sample"));
	EXPECT_FALSE(manager.startComponent("sample"));
	EXPECT_TRUE(manager.stopComponent("sample"));
	EXPECT_FALSE(manager.stopComponent("sample"));
	EXPECT_TRUE(manager.unloadComponent("sample"));
	EXPECT_FALSE(manager.unloadComponent("sample"));
}


TEST_F(ComponentManagerTest,DependenciesSimple) {
	using Susi::Util::Any;
	using namespace Susi::System;
	Any::Object config{{"sample1","foobar"},{"sample2","foobar"}};
	ComponentManager manager{config};
	manager.registerComponent("sample1",[](ComponentManager * mgr, Any config){
		return std::shared_ptr<Component>{new SampleComponent{config}};
	});
	manager.registerComponent("sample2",[](ComponentManager * mgr, Any config){
		return std::shared_ptr<Component>{new SampleComponent{config}};
	});
	manager.registerDependency("sample2","sample1");
	EXPECT_TRUE(manager.startComponent("sample2"));
	EXPECT_FALSE(manager.startComponent("sample1")); // it is allready loaded
}


TEST_F(ComponentManagerTest,DependenciesExtended) {
	using Susi::Util::Any;
	using namespace Susi::System;
	Any::Object config{{"sample1","foobar"},{"sample2","foobar"},{"sample3","foobar"}};
	ComponentManager manager{config};
	manager.registerComponent("sample1",[](ComponentManager * mgr, Any config){
		return std::shared_ptr<Component>{new SampleComponent{config}};
	});
	manager.registerComponent("sample2",[](ComponentManager * mgr, Any config){
		return std::shared_ptr<Component>{new SampleComponent{config}};
	});
	manager.registerComponent("sample3",[](ComponentManager * mgr, Any config){
		return std::shared_ptr<Component>{new SampleComponent{config}};
	});
	manager.registerDependency("sample3","sample2");
	manager.registerDependency("sample2","sample1");

	EXPECT_TRUE(manager.startComponent("sample3"));
	EXPECT_FALSE(manager.startComponent("sample2")); // it is allready loaded
	EXPECT_FALSE(manager.startComponent("sample1")); // it is allready loaded
}

TEST_F(ComponentManagerTest,NoConfigCantLoad) {
	using Susi::Util::Any;
	using namespace Susi::System;
	Any::Object config{};
	ComponentManager manager{config};
	manager.registerComponent("sample1",[](ComponentManager * mgr, Any config){
		return std::shared_ptr<Component>{new SampleComponent{config}};
	});
	EXPECT_FALSE(manager.startComponent("sample1"));
}

TEST_F(ComponentManagerTest,NoConfigCantLoadDep) {
	using Susi::Util::Any;
	using namespace Susi::System;
	Any::Object config{{"sample1","foobar"}};
	ComponentManager manager{config};
	manager.registerComponent("sample1",[](ComponentManager * mgr, Any config){
		return std::shared_ptr<Component>{new SampleComponent{config}};
	});
	manager.registerComponent("sample2",[](ComponentManager * mgr, Any config){
		return std::shared_ptr<Component>{new SampleComponent{config}};
	});
	manager.registerDependency("sample1","sample2");
	EXPECT_FALSE(manager.startComponent("sample1"));
}

TEST_F(ComponentManagerTest,Stop) {
	using Susi::Util::Any;
	using namespace Susi::System;
	Any::Object config{{"sample1","foobar"},{"sample2","foobar"}};
	ComponentManager manager{config};
	manager.registerComponent("sample1",[](ComponentManager * mgr, Any config){
		return std::shared_ptr<Component>{new SampleComponent{config}};
	});
	manager.registerComponent("sample2",[](ComponentManager * mgr, Any config){
		return std::shared_ptr<Component>{new SampleComponent{config}};
	});
	manager.registerDependency("sample1","sample2");
	EXPECT_TRUE(manager.startComponent("sample1"));
	EXPECT_TRUE(manager.stopComponent("sample2"));
	EXPECT_FALSE(manager.stopComponent("sample1"));
}

TEST_F(ComponentManagerTest,Get) {
	using Susi::Util::Any;
	using namespace Susi::System;
	Any::Object config{{"sample1","foobar"}};
	ComponentManager manager{config};
	manager.registerComponent("sample1",[](ComponentManager * mgr, Any config){
		return std::shared_ptr<Component>{new SampleComponent{config}};
	});
	EXPECT_TRUE(manager.startComponent("sample1"));
	EXPECT_TRUE(manager.getComponent("sample1").get() != nullptr);
	EXPECT_TRUE(manager.getComponent<SampleComponent>("sample1").get() != nullptr);
	class SpecialSampleComponent : public SampleComponent {};
	EXPECT_TRUE(manager.getComponent<SpecialSampleComponent>("sample1").get() == nullptr);
}



