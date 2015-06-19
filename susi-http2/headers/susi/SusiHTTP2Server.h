#include "susi/SusiClient.h"

#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif // HAVE_UNISTD_H
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif // HAVE_FCNTL_H
#include <iostream>
#include <string>

#include <nghttp2/asio_http2_server.h>
#include <boost/asio/ssl.hpp>

#include "susi/MimeTable.h"

using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;

class SusiHTTP2Server {

public:
    SusiHTTP2Server(Susi::SusiClient & susi, std::string key, std::string cert, std::string addr, short port, std::string docRoot, std::vector<std::string> pushFiles) : 
        susi_{susi}, 
        addr{addr}, 
        port{port},
        pushFiles{pushFiles} {
        this->docRoot = docRoot;
        tls.use_private_key_file(key, boost::asio::ssl::context::pem);
        tls.use_certificate_chain_file(cert);
        configure_tls_context_easy(ec, tls);
        server.num_threads(4);
        server.handle("/",[this](const request &req, const response &res){
            assetsHandler(req,res);
        });
        server.handle("/api/",[this](const request &req, const response &res){
            apiHandler(req,res);
        });
    }

    void start(){
        runloop = std::move(std::thread{[this](){
            if (server.listen_and_serve(ec, tls, addr, std::to_string(port))) {
                std::cerr << "error: " << ec.message() << std::endl;
            }
        }});
    }

    void stop(){
        server.stop();
    }

    void join(){
        server.join();
        if(runloop.joinable())runloop.join();
    }
    ~SusiHTTP2Server(){
        stop();
        join();
    }

protected:
    Susi::SusiClient & susi_;
    std::string addr;
    short port;
    boost::asio::ssl::context tls{boost::asio::ssl::context::tlsv12_server};
    http2 server;
    boost::system::error_code ec;
    std::thread runloop;
    std::string docRoot;
    std::vector<std::string> pushFiles;
    Susi::MimeTable mimeTable;

    void assetsHandler(const request &req, const response &res){
        auto headers = header_map();
        sessionHandling(req,headers);
        auto path = percent_decode(req.uri().path);
        std::cout<<"assets request for "<<path<<std::endl;
        if (!check_path(path)) {
            res.write_head(404);
            res.end();
            return;
        }
        if (path == "/") {
            path = "/index.html";
            handlePushing(res);
        }
        path = docRoot + path;
        writeFileToResponse(res,path,headers);
    }

    void apiHandler(const request &req, const response &res){
        std::cout<<"got api request"<<std::endl;
        BSON::Array requestHeaders{};
        for(auto & kv : req.header()){
            BSON::Value header = BSON::Object{
                {kv.first,kv.second.value}
            };
            requestHeaders.push_back(header);
        }
        BSON::Value requestData = BSON::Object{
            {"path",percent_decode(req.uri().path)},
            {"requestHeaders",requestHeaders},
            {"responseHeaders",BSON::Array{}},
            {"body",""},
            {"status",404}
        };
        auto closed = std::make_shared<bool>();
        *closed = false;
        res.on_close([closed](uint32_t error_code) {
            std::cout<<"request is now closed"<<std::endl;
            *closed = true;
        });
        std::cout<<"publish "<<requestData.toJSON()<<std::endl;
        susi_.publish("http::api::request",requestData,[this,&req,&res,closed](const BSON::Value & evt){
            std::cout<<"got ack for api request"<<std::endl;
            if(!(*closed)){
                std::cout<<"its not closed"<<std::endl;
                auto headers = header_map();
                sessionHandling(req,headers);
                for(auto headerPair : evt["payload"]["responseHeaders"].getArray()){
                    if(headerPair.isObject()){
                        for(auto & kv : headerPair){
                            auto key = kv.first;
                            auto value = header_value{kv.second.getString()};
                            headers.insert({key,value});
                        }
                    }
                }
                std::cout<<"ok, write headers and body..."<<std::endl;
                res.write_head(evt["payload"]["status"].getInt64(),headers);
                res.end(evt["payload"]["body"].getString());
            }
        });

    }

    void handlePushing(const response &res){
        for(auto & file : pushFiles){
            std::cout<<"pushing "<<file<<std::endl;
            auto push = res.push(ec, "GET", file);
            if (!ec) {
                auto headers = header_map();
                writeFileToResponse(*push,docRoot+file,headers);
            }
        }
    }

    void sessionHandling(const request &req, header_map & responseHeaders){
        auto sessionId = getSessionCookie(req);
        if(sessionId==""){
            auto id = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
            setSessionCookie(responseHeaders,id);
            std::cout<<"set a session cookie!"<<std::endl;
            //susi_.publish("http::session::new",id);
        }else{
            std::cout<<"got a session cookie: "<<sessionId<<std::endl;
        }
    }

    void writeFileToResponse(const response &res, std::string filename, header_map & header){
        auto fd = open(filename.c_str(), O_RDONLY);
        if (fd == -1) {
            res.write_head(404);
            res.end();
            return;
        }
        struct stat stbuf;
        if (stat(filename.c_str(), &stbuf) == 0) {
            header.emplace("content-length",
                           header_value{std::to_string(stbuf.st_size)});
            header.emplace("last-modified",
                           header_value{http_date(stbuf.st_mtime)});
            header.emplace("content-type",
                   header_value{mimeTable.getContentType(filename)});
        }
        res.write_head(200, std::move(header));
        res.end(file_generator_from_fd(fd));
    }

    std::string getSessionCookie(const request &req){
        auto & headers = req.header();
        for(auto & kv : headers){
            if(kv.first == "cookie"){
                if(kv.second.value.find("susisession") == 0){
                    return kv.second.value.substr(12);
                }
            }
        }
        return "";
    }

    void setSessionCookie(header_map & headers,std::string sessionId){
        headers.emplace("Set-Cookie",header_value{"susisession="+sessionId+";Path=/;"});
    }

};

