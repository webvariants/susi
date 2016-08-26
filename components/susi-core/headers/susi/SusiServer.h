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

#ifndef __SUSISERVER__
#define __SUSISERVER__

#include "susi/FramingServer.h"
#include "susi/LineFramer.h"
#include <regex>
#include <chrono>
#include <bson/Value.h>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "susi/SSLTCPServer.h"

namespace Susi {

class SusiServer : public FramingServer<LineFramer, SSLTCPServer> {
  protected:
    std::map<std::string, std::vector<int>> consumers;  //topic to list of client id's
    std::map<std::string, std::vector<int>> processors; //topic to list of client id's

    struct PublishProcess {
        PublishProcess(boost::asio::io_service & io) : timeout{io} {}
        boost::asio::deadline_timer timeout;
        BSON::Value lastState;
        std::vector<int> processors;    //list of client id's
        std::vector<int> consumers;     //list of client id's
        size_t nextProcessor = 0;
        int publisher;                  //client id of the publisher
    };

    std::map<std::string, std::shared_ptr<PublishProcess>> publishProcesses; //event id to publish process

  public:
    SusiServer(short port, std::string keyFile, std::string certificateFile);

    virtual ~SusiServer();

    virtual void onConnect(int id) override;
    virtual void onClose(int id) override;
    virtual void onFrame(int id, std::string & frame) override;

  protected:

    bool validateFrame(BSON::Value & doc);
    void send(int id, BSON::Value & doc);
    void registerConsumer(std::string & topic, int id);
    void registerProcessor(std::string & topic, int id);
    void unregisterConsumer(std::string & topic, int id);
    void unregisterProcessor(std::string & topic, int id);
    void publish(BSON::Value & event, int publisher);
    void ack(BSON::Value & event, int acker);
    void dismiss(BSON::Value & event, int acker);
    bool contains(std::vector<int> & vec, int elem);
    void checkAndReactToSusiEvents(BSON::Value & event);
    bool checkForNoAckHeader(BSON::Value & event);
    bool checkForNoConsumerHeader(BSON::Value & event);
    bool checkForNoProcessorHeader(BSON::Value & event);
    bool checkForHeader(BSON::Value & event, const std::string & key, const std::string & value);
    int64_t checkForTimeoutHeader(BSON::Value & event);
};


}

#endif // __SUSISERVER__
