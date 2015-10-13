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

#ifndef __FRAMINGCLIENT__
#define __FRAMINGCLIENT__

#include <map>
#include <memory>

#include <iostream>

namespace Susi {
template <class Framer, class Client>
class FramingClient : public Client {
  protected:
    std::shared_ptr<Framer> framer;
  public:
    FramingClient(std::string host, short port, std::string keyFile, std::string certificateFile) :
        Client{host, port, keyFile, certificateFile} {
        framer = std::make_shared<Framer>([this](std::string & frame) {onFrame(frame);});
    }
    virtual ~FramingClient() {}

    virtual void onData(char *data, size_t len) override {
        framer->collect(data, len);
    }

    virtual void onFrame(std::string & msg) = 0;

};

}

#endif // __FRAMINGCLIENT__
