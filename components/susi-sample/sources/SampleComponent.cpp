#include "susi/SampleComponent.h"

Susi::SampleComponent::SampleComponent(Susi::SusiClient & susi, const BSON::Value & config) : _susi{susi}, _config{config} {

    susi.registerConsumer("samplecomonent::init", [this](Susi::SharedEventPtr event){
        std::cout<<"SampleComponent: got init event, here its payload:\n" << event->payload.toJSON() << std::endl;
    });

    auto initEvent = susi.createEvent("samplecomonent::init");
    initEvent->payload = _config;
    _susi.publish(std::move(initEvent));

    std::cout << "SampleComponent is started" << std::endl;
}

Susi::SampleComponent::~SampleComponent(){
    join();
}

void Susi::SampleComponent::join(){
    _susi.join();
}
