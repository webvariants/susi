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

#ifndef __FRAMINGSERVER__
#define __FRAMINGSERVER__

#include <map>
#include <string>

namespace Susi {

template <class Framer, class BaseServer>
class FramingServer : public BaseServer {
  protected:
    std::map<int, Framer> framers;
  public:
    FramingServer(short port, std::string keyFile, std::string certificateFile) :
        BaseServer{port, keyFile, certificateFile} {}
    virtual ~FramingServer() {}

    virtual void onConnect(int id) override {
        framers[id] = Framer{[this, id](std::string & msg) {
            onFrame(id, msg);
        }};
    }

    virtual void onData(int id, char *data, size_t len) override {
        framers[id].collect(data, len);
    }

    virtual void onFrame(int id, std::string & msg) = 0;

    virtual void onClose(int id) override {
        framers.erase(id);
    }
};

}

#endif // __FRAMINGSERVER__
