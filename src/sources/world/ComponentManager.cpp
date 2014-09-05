#include "world/ComponentManager.h"

void Susi::System::ComponentManager::registerComponent(std::string name, RegisterFunction func){
	registerFunctions.emplace(name,std::move(func));
}
void Susi::System::ComponentManager::registerDependency(std::string subject, std::string dependency){
	auto & deps = dependencies[subject];
	deps.push_back(dependency);
	auto & iDeps = inverseDependencies[dependency];
	iDeps.push_back(subject);
}

bool Susi::System::ComponentManager::loadComponent(std::string name){
	if(components.find(name) == components.end() && registerFunctions[name]){
		ComponentData data;
		if(config[name].isNull())return false;
		data.component = registerFunctions[name](this,config[name]);
		components[name] = data;
		return true;
	}
	return false;
}

bool Susi::System::ComponentManager::unloadComponent(std::string name){
	if(components.find(name) == components.end()){
		return false;
	}
	components[name].component->stop();
	components.erase(name);
	return true;
}

bool Susi::System::ComponentManager::startComponent(std::string name){
	if(components.find(name) == components.end() && !loadComponent(name)){
		return false;
	}
	auto & data = components[name];
	if(data.running){
		return false;
	}
	for(std::string & dep : dependencies[name]){
		startComponent(dep);
		if(components.find(dep) == components.end() || !components[dep].running){
			return false;
		}
	}
	data.component->start();
	data.running = true;

	return true;
}

bool Susi::System::ComponentManager::stopComponent(std::string name){
	if(components.find(name) == components.end() || components[name].running == false) {
		return false;
	}
	for(std::string & iDep : inverseDependencies[name]){
		stopComponent(iDep);
	}
	components[name].component->stop();
	components[name].running = false;

	return true;
}


bool Susi::System::ComponentManager::startAll() {
	bool result = true;
	for(auto kv: config) {
		result = result && startComponent(kv.first);
	}
	return result;
}

bool Susi::System::ComponentManager::stopAll() {
	bool result = true;
	for(auto kv: config) {
		result = result && stopComponent(kv.first);
	}
	return result;
}
