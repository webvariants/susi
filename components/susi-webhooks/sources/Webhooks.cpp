#include "susi/Webhooks.h"
#include <boost/asio.hpp>
using boost::asio::ip::tcp;

Webhooks::Webhooks(std::string addr,short port, std::string key, std::string cert) :
  susi_{new Susi::SusiClient{addr,port,key,cert}} {
    susi_->registerProcessor("webhooks::send",[this](Susi::EventPtr event){
      std::cout<<"got webhook send request"<<std::endl;
      const std::string & host = event->payload["host"];
      const std::string & port = event->payload["port"];
      const std::string & path = event->payload["path"];
      const auto & body = event->payload["body"].toJSON();
      tcp::resolver resolver{io_service_};
      tcp::resolver::query query{host, port};
      tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
      tcp::socket socket(io_service_);
      boost::asio::connect(socket, endpoint_iterator);
      boost::asio::streambuf request;
      std::ostream request_stream(&request);
      request_stream << "POST " << path << " HTTP/1.0\r\n";
      request_stream << "Host: " << host << "\r\n";
      request_stream << "Accept: */*\r\n";
      request_stream << "Connection: close\r\n";
      request_stream << "Content-Length: "<<body.size()<<"\r\n\r\n";

      request_stream << body <<"\r\n\r\n";
      boost::asio::write(socket, request);
      boost::asio::streambuf response;
      boost::asio::read_until(socket, response, "\r\n");

      std::istream response_stream(&response);
      std::string http_version;
      response_stream >> http_version;
      unsigned int status_code;
      response_stream >> status_code;
      std::string status_message;
      std::getline(response_stream, status_message);
      if (!response_stream || http_version.substr(0, 5) != "HTTP/" || status_code != 200) {
        event->headers.push_back({"Error","Invalid response"});
        event->payload["statuscode"] = (BSON::int64)status_code;
        susi_->ack(std::move(event));
        return;
      }

      boost::asio::read_until(socket, response, "\r\n\r\n");
      std::string header;
      event->payload["responseHeaders"] = BSON::Array{};
      while (std::getline(response_stream, header) && header != "\r"){
        event->payload["responseHeaders"].push_back(header);
      }
      std::stringstream res;
      if (response.size() > 0){
        res << &response;
      }
      boost::system::error_code error;
      while (boost::asio::read(socket, response,
           boost::asio::transfer_at_least(1), error)){
        res << &response;
      }
      event->payload["responseBody"] = res.str();
      susi_->ack(std::move(event));
    });
}

void Webhooks::join(){
	susi_->join();
}
