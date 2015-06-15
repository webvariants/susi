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

using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;

class SusiHTTP2Server {

public:
	SusiHTTP2Server(Susi::SusiClient & susi, std::string key, std::string cert, std::string addr, short port, std::string docRoot) : susi_{susi} {
		this->docRoot = docRoot;
		tls.use_private_key_file(key, boost::asio::ssl::context::pem);
		tls.use_certificate_chain_file(cert);
		configure_tls_context_easy(ec, tls);
		server.num_threads(4);
		server.handle("/assets/",[this](const request &req, const response &res){
			assetsHandler(req,res);
		});
		runloop = std::move(std::thread{[this,addr,port](){
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
	std::string dbPath_;
	boost::asio::ssl::context tls{boost::asio::ssl::context::tlsv12_server};
	http2 server;
	boost::system::error_code ec;
	std::thread runloop;
	std::string docRoot;
	void assetsHandler(const request &req, const response &res){
		auto path = percent_decode(req.uri().path);
		if (!check_path(path)) {
			res.write_head(404);
			res.end();
			return;
		}

		if (path == "/") {
			path = "/index.html";
		}

		path = docRoot + path;
		auto fd = open(path.c_str(), O_RDONLY);
		if (fd == -1) {
			res.write_head(404);
			res.end();
			return;
		}

		auto header = header_map();

		struct stat stbuf;
		if (stat(path.c_str(), &stbuf) == 0) {
		header.emplace("content-length",
		               header_value{std::to_string(stbuf.st_size)});
		header.emplace("last-modified",
		               header_value{http_date(stbuf.st_mtime)});
		}
		res.write_head(200, std::move(header));
		res.end(file_generator_from_fd(fd));
	}



};

