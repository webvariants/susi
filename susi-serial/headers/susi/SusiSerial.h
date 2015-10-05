#include "susi/SusiClient.h"
#include <string> 
#include <iostream> 
#include <boost/asio.hpp> 
#include <boost/system/system_error.hpp> 


class SusiSerial {

public:
    SusiSerial(std::string addr,short port, std::string key, std::string cert);
    void join();
    void initPort(const std::string & id, const std::string & portname, int baudrate);
protected:
    std::shared_ptr<Susi::SusiClient> _susi;
    std::map<std::string, std::shared_ptr<boost::asio::serial_port>> _ports;
    boost::asio::io_service _ioservice;
};

