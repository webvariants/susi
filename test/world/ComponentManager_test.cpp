#include "gtest/gtest.h"
#include "world/ComponentManager.h"

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
	manager.registerComponent("sample",[](ComponentManager *mgr, Any config){
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
	manager.registerComponent("sample1",[](ComponentManager *mgr, Any config){
		return std::shared_ptr<Component>{new SampleComponent{config}};
	});
	manager.registerComponent("sample2",[](ComponentManager *mgr, Any config){
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
	manager.registerComponent("sample1",[](ComponentManager *mgr, Any config){
		return std::shared_ptr<Component>{new SampleComponent{config}};
	});
	manager.registerComponent("sample2",[](ComponentManager *mgr, Any config){
		return std::shared_ptr<Component>{new SampleComponent{config}};
	});
	manager.registerComponent("sample3",[](ComponentManager *mgr, Any config){
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
	manager.registerComponent("sample1",[](ComponentManager *mgr, Any config){
		return std::shared_ptr<Component>{new SampleComponent{config}};
	});
	EXPECT_FALSE(manager.startComponent("sample1"));
}

TEST_F(ComponentManagerTest,NoConfigCantLoadDep) {
	using Susi::Util::Any;
	using namespace Susi::System;
	Any::Object config{{"sample1","foobar"}};
	ComponentManager manager{config};
	manager.registerComponent("sample1",[](ComponentManager *mgr, Any config){
		return std::shared_ptr<Component>{new SampleComponent{config}};
	});
	manager.registerComponent("sample2",[](ComponentManager *mgr, Any config){
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
	manager.registerComponent("sample1",[](ComponentManager *mgr, Any config){
		return std::shared_ptr<Component>{new SampleComponent{config}};
	});
	manager.registerComponent("sample2",[](ComponentManager *mgr, Any config){
		return std::shared_ptr<Component>{new SampleComponent{config}};
	});
	manager.registerDependency("sample1","sample2");
	EXPECT_TRUE(manager.startComponent("sample1"));
	EXPECT_TRUE(manager.stopComponent("sample2"));
	EXPECT_FALSE(manager.stopComponent("sample1"));
}

TEST_F(ComponentManagerTest,Get){
	using Susi::Util::Any;
	using namespace Susi::System;
	class SpecialComponent : public SampleComponent {
	public:
		SpecialComponent(Susi::Util::Any config) : SampleComponent{config} {}
	};
	class SuperSpecialComponent : public SpecialComponent {};
	Any::Object config{{"sample","foobar"}};
	ComponentManager manager{config};
	manager.registerComponent("sample",[](ComponentManager * mgr, Any conf){
		return std::shared_ptr<Component>{new SpecialComponent{conf}};
	});
	EXPECT_TRUE(manager.startComponent("sample"));
	std::shared_ptr<Susi::System::Component> c1 = manager.getComponent("sample");
	EXPECT_NE(nullptr,c1.get()); // no template args -> type is component
	std::shared_ptr<SpecialComponent> c2 = manager.getComponent<SpecialComponent>("sample");
	EXPECT_NE(nullptr,c2.get()); // template arg is SpecialComponent -> correct
	std::shared_ptr<SuperSpecialComponent> c3 = manager.getComponent<SuperSpecialComponent>("sample");
	EXPECT_EQ(nullptr,c3.get()); // type SuperSpecialComponent requested -> returning nullptr
}


	