#include "susi/SusiClient.h"

int main() {

    Susi::SusiClient client{"localhost", 4000, "key1.pem", "cert1.pem"};

    client.registerProcessor("test-topic", [](Susi::EventPtr event) {
        std::cout << "processor 1" << std::endl;
    });

    client.registerProcessor("test-topic", [&client](Susi::EventPtr event) {
        std::cout << "processor 2" << std::endl;
        client.dismiss(std::move(event));
    });

    client.registerProcessor("test-topic", [&client](Susi::EventPtr event) {
        std::cout << "processor 3" << std::endl;
    });

    client.registerConsumer("test-topic", [](Susi::SharedEventPtr event) {
        std::cout << "consumer 1" << std::endl;
    });

    auto event = client.createEvent("test-topic");
    event->payload = BSON::Object{
        {"value", 123},
        {"name", "foo"}
    };
    client.publish(std::move(event), [](Susi::SharedEventPtr event) {
        std::cout << "finish callback" << std::endl;
        for (const auto & kv : event->headers) {
            if (kv.first == "error" && kv.second == "dismissed") {
                std::cout << "event got dismissed" << std::endl;
            }
        }
    });

    client.join();

    return 0;
}