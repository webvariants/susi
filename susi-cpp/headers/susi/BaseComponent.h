/*
 * Copyright (c) 2015, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Tino Rusch (tino.rusch@webvariants.de)
 */

#ifndef __BASECOMPONENT__
#define __BASECOMPONENT__

#include "susi/SusiClient.h"

namespace Susi {
    class BaseComponent {
    protected:
        Susi::SusiClient & client_;
    public:
        BaseComponent(Susi::SusiClient & client) : client_{client} {}

        void publish(std::string topic, BSON::Value payload, Consumer finishCallback = Consumer{}){
            client.publish(topic,payload,finishCallback);
        }
        std::uint64_t registerProcessor(std::string topic, Processor processor){
            return client.registerProcessor(topic,processor);
        }
        std::uint64_t registerConsumer(std::string topic, Consumer consumer){
            return client.registerConsumer(topic,consumer);
        }
        bool unregisterProcessor(std::uint64_t id){
            return client.unregisterProcessor(id);
        }
        bool unregisterConsumer(std::uint64_t id){
            return client.unregisterConsumer(id);
        }

    };

}

#endif // __BASECOMPONENT__
