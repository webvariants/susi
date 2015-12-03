/**
 * Usage example SUSI CPP Engine
 */
#include "susi/SusiClient.h" //at first, include the susi client header

int main() {

    // create a client to your susi server
    // The arguments are: network-address, portnumber, ssl-key file, ssl-certificate file
    Susi::SusiClient client{"localhost", 4000, "key1.pem", "cert1.pem"};

    // This is the syntax to register event processors, they are called sequentially
    // so the order matters. The first argument is the event topic to listen to. It can
    // also be an regular expression. The second argument is a function-object with
    // the signature "void yourname(Susi::EventPtr event)". Thanks to c++11 you can pass
    // normal function objects, all callables with this signature (i.e. structs with member function void()(EventPtr))),
    // lambdas, or event c function pointers.
    // The processor can now manipulate the event. It will automatically passed back to the eventsystem,
    // once it gets destructed. Susi::EventPtr is a typedef for std::unique_ptr<Susi::Event>, so you can only move it
    // and must dereference it with '->'.
    //
    // In this example we register an processor which matches all events. It sets the payload to an empty object.
    client.registerProcessor(".*", [](Susi::EventPtr event) {
        if (event->payload.isObject()) {
            std::cout << "processor by regexp, found an object as event payload: " << event->payload.toJSON() << std::endl;
        } else {
            std::cout << "processor by regexp, set payload of event to an empty object" << std::endl;
            event->payload = BSON::Object{};
        }
    });

    // this processor sets an payload property, and dismisses the event, so it prevents all following processors to be called.
    client.registerProcessor("test-topic", [&client](Susi::EventPtr event) {
        std::cout << "processor by eventname: set payload property" << std::endl;
        event->payload["processor1"] = true;
        client.dismiss(std::move(event));
    });
    // this processor also sets an payload property, but it will never be called, due to the last processor which dismissed the event.
    client.registerProcessor("test-topic", [&client](Susi::EventPtr event) {
        std::cout << "processor by eventname: set payload property" << std::endl;
        event->payload["processor2"] = true;
    });

    // this is a consumer, it gets called when all processors are finished.
    // The signature of a consumer is different. It takes an Susi::SharedEventPtr which is defacto an std::shared_ptr<Susi::Event>
    // Consumers are called concurrently, so do NOT modify the event.
    client.registerConsumer("test-topic", [](Susi::SharedEventPtr event) {
        std::cout << "consumer: event-payload: " << event->payload.toJSON() << std::endl;
    });

    // So lets publish an event!
    // At first we need to create one
    auto event = client.createEvent("test-topic");
    event->payload = BSON::Object{
        {"client-data",123.2},
    };
    // This is the publish syntax, you may add an second argument, which is a one-time consumer for this event,
    // so after all processors, this callback will be called, so that you can react to processor data, attached to the event.
    client.publish(std::move(event), [](Susi::SharedEventPtr event) {
        std::cout << "finish callback" << std::endl;
        if (event->hasDismissedHeader()) {
            std::cout << "event got dismissed" << std::endl;
        }
        std::cout << event->payload.toJSON() << std::endl;
    });

    client.join();

    return 0;
}