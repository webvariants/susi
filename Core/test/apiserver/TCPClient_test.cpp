#include "gtest/gtest.h"
#include "apiserver/TCPClient.h"
#include <mutex>
#include <condition_variable>
#include <vector>
#include <utility>

#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"
#include "Poco/Net/SocketNotification.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/NObserver.h"


class TCPClientTest : public ::testing::Test {
protected:

	class EchoServer {
	protected:
		class TestServerHandler {
		public:
		      TestServerHandler(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor):
		            _socket(socket),
		            _reactor(reactor),
		            _pBuffer(new char[BUFFER_SIZE])
		      {
		            _reactor.addEventHandler(_socket, Poco::NObserver<TestServerHandler, Poco::Net::ReadableNotification>(*this, &TestServerHandler::onReadable));
		            _reactor.addEventHandler(_socket, Poco::NObserver<TestServerHandler, Poco::Net::ShutdownNotification>(*this, &TestServerHandler::onShutdown));
		      }
		      
		      ~TestServerHandler() {
		            _reactor.removeEventHandler(_socket, Poco::NObserver<TestServerHandler, Poco::Net::ReadableNotification>(*this, &TestServerHandler::onReadable));
		            _reactor.removeEventHandler(_socket, Poco::NObserver<TestServerHandler, Poco::Net::ShutdownNotification>(*this, &TestServerHandler::onShutdown));
		            delete [] _pBuffer;
		      }
		      
		      void onReadable(const Poco::AutoPtr<Poco::Net::ReadableNotification>& pNf) {
		            int n = _socket.receiveBytes(_pBuffer, BUFFER_SIZE);
		            if (n > 0)
		                  _socket.sendBytes(_pBuffer, n);
		            else
		                  delete this;
		      }
		      
		      void onShutdown(const Poco::AutoPtr<Poco::Net::ShutdownNotification>& pNf) {
		            delete this;
		      }
		      
		private:
		      enum
		      {
		            BUFFER_SIZE = 1024
		      };
		      
		      Poco::Net::StreamSocket   _socket;
		      Poco::Net::SocketReactor& _reactor;
		      char*      			    _pBuffer;
		};
		Poco::Net::ServerSocket svs{};
		Poco::Net::SocketReactor reactor;
		std::shared_ptr<Poco::Net::SocketAcceptor<TestServerHandler>> acceptor;
		std::thread runloop;
	public: 
		EchoServer(){
			svs.bind(Poco::Net::SocketAddress{"localhost:2222"},true);
			svs.listen();
			acceptor.reset(new Poco::Net::SocketAcceptor<TestServerHandler>{svs, reactor});
			runloop = std::move(std::thread{[this](){
				reactor.run();
			}});
		}
		~EchoServer(){
			reactor.stop();
			if(runloop.joinable())runloop.join();
		}
	};

	std::shared_ptr<EchoServer> echoServer;

	virtual void SetUp() override {
		echoServer = std::make_shared<EchoServer>();
	}
	virtual void TearDown() override {
		echoServer.reset();
	}

};

TEST_F(TCPClientTest, Basic) {
	struct TestTCPClient : public Susi::Api::TCPClient {
		TestTCPClient(std::string addr) : Susi::Api::TCPClient{addr} {}
		virtual void onData(std::string & data) override {
			EXPECT_EQ("foobar",data);
		}
	};
	TestTCPClient client{"localhost:2222"};
	client.send("foobar");
	std::this_thread::sleep_for(std::chrono::milliseconds{100});
	client.close();
	client.join();
}

/*TEST_F(TCPClientTest, Reconnect) {
	struct TestTCPClient : public Susi::Api::TCPClient {
		std::mutex mutex;
		std::condition_variable cond;
		bool done = false;
		TestTCPClient(std::string addr) : Susi::Api::TCPClient{addr} {
			Susi::Api::TCPClient::setMaxReconnectCount(5);
		}
		virtual void onData(std::string & data) override {
			EXPECT_EQ("foobar",data);
			done = true;
			cond.notify_all();
		}
	};
	TestTCPClient client{"localhost:2222"};
	client.send("foobar");
	{
		std::unique_lock<std::mutex> lock{client.mutex};
		client.cond.wait_for(lock, std::chrono::milliseconds{500},[&client]{return client.done;});
	}
	EXPECT_TRUE(client.done);
	client.done = false;

	//restart echoserver
	echoServer.reset();
	echoServer = std::make_shared<EchoServer>();

	client.send("foobar");
	{
		std::unique_lock<std::mutex> lock{client.mutex};
		client.cond.wait_for(lock, std::chrono::milliseconds{5000}, [&client]{return client.done;});
	}
	EXPECT_TRUE(client.done);
}*/