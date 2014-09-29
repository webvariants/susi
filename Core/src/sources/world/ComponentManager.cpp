#include "world/ComponentManager.h"

void Susi::System::ComponentManager::registerComponent( std::string name, RegisterFunction func ) {
    registerFunctions.emplace( name,std::move( func ) );
}
void Susi::System::ComponentManager::registerDependency( std::string subject, std::string dependency ) {
    auto & deps = dependencies[subject];
    deps.push_back( dependency );
    auto & iDeps = inverseDependencies[dependency];
    iDeps.push_back( subject );
}

bool Susi::System::ComponentManager::loadComponent( std::string name ) {
    if( components.find( name ) == components.end() && registerFunctions[name] ) {
        ComponentData data;
        if( config[name].isNull() ) {
            Susi::Logger::debug( Susi::Util::Any{config}.toJSONString() );
            Susi::Logger::debug( "cant find config for component "+name );
            return false;
        }
        data.component = registerFunctions[name]( this,config[name] );
        components[name] = data;
        Susi::Logger::debug( "loaded component "+name+"!" );
        return true;
    }
    else {
        //Susi::Logger::debug("cant find register-function for component "+name);
    }

    return false;
}

bool Susi::System::ComponentManager::unloadComponent( std::string name ) {
    if( components.find( name ) == components.end() ) {
        return false;
    }
    components[name].component->stop();
    components.erase( name );
    return true;
}

bool Susi::System::ComponentManager::startComponent( std::string name ) {
    Susi::Logger::debug( "starting component "+name+"..." );
    if( components.find( name ) == components.end() && !loadComponent( name ) ) {
        Susi::Logger::debug( "can't start component "+name+", component is not loadable." );
        return false;
    }
    auto & data = components[name];
    if( data.running ) {
        Susi::Logger::debug( "can't start component "+name+", component is allready ruuning." );
        return false;
    }
    for( std::string & dep : dependencies[name] ) {
        Susi::Logger::debug( "need dependency "+dep );
        startComponent( dep );
        if( components.find( dep ) == components.end() || !components[dep].running ) {
            Susi::Logger::debug( "can't start component "+name+", dependency "+dep+" is not startable." );
            return false;
        }
    }
    data.component->start();
    data.running = true;
    Susi::Logger::debug( "started component "+name+"!" );
    return true;
}

bool Susi::System::ComponentManager::stopComponent( std::string name ) {
    if( components.find( name ) == components.end() || components[name].running == false ) {
        return false;
    }
    for( std::string & iDep : inverseDependencies[name] ) {
        stopComponent( iDep );
    }
    components[name].component->stop();
    components[name].running = false;

    return true;
}


bool Susi::System::ComponentManager::startAll() {
    bool result = true;

    for( auto kv: config ) {
        startComponent( kv.first );
    }

    std::string runningComponents = "";
    for( auto kv : components ) {
        if( kv.second.running ) {
            runningComponents+=kv.first+" ";
        }
    }
    Susi::Logger::info( "successfully started these components: "+runningComponents );

    // test all started
    for( auto kv: config ) {
        auto & data = components[kv.first];
        result = result & data.running;
    }
    return result;
}

bool Susi::System::ComponentManager::stopAll() {
    bool result = true;
    for( auto kv: config ) {
        stopComponent( kv.first );
    }

    // test all stopped
    for( auto kv: config ) {
        auto & data = components[kv.first];
        result = result & !data.running;
    }
    return result;
}
