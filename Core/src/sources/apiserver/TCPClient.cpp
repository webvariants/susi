#include "apiserver/TCPClient.h"



Susi::Api::TCPClient::TCPClient( std::string address ) : sa {address} {
    startRunloop();
}

Susi::Api::TCPClient::TCPClient( TCPClient && other ) {
    isClosed.store(other.isClosed.load());
    std::swap( sa,other.sa );
    std::swap( sock,other.sock );
    std::swap( runloop,other.runloop );
}


void Susi::Api::TCPClient::close() {
    if( !isClosed.load() ) {
        isClosed.store(true);
        try {
            sock.shutdown();
            sock.close();
            join();
            onClose();
        }
        catch( const std::exception & e ) {
            //std::cout<<"error while closing/joining: "<<e.what()<<std::endl;
            onClose();
        }
    }
}
void Susi::Api::TCPClient::send( std::string msg ) {
    try{
        sock.sendBytes( msg.c_str(),msg.size() );
    }catch(std::exception & e){
        //std::cout<<"error while sending: "<<e.what()<<std::endl;
        //sendbuff += msg;
    }
}
void Susi::Api::TCPClient::join() {
    if(runloop.joinable())runloop.join();
}
Susi::Api::TCPClient::~TCPClient() {
    close();
    join();
}

void Susi::Api::TCPClient::startRunloop(){
    sock.connect(sa);
    onConnect();
    sock.setReceiveTimeout( Poco::Timespan{0,100000} );
    runloop = std::move( std::thread{
        [this]() {
            char buff[1024];
            int bs;
            while( !isClosed.load() ) { // data chunk loop
                try {
                    ////std::cout<<"wait for bytes"<<std::endl;
                    bs = sock.receiveBytes( buff,1024 );
                    ////std::cout<<"got "<<bs<<std::endl;
                    if( bs<=0 ) {
                        throw std::runtime_error{"got zero bytes -> connection reset by peer"};
                    }
                    std::string data {buff,static_cast<size_t>( bs )};
                    onData( data );
                }
                catch( const Poco::TimeoutException & e ) {
                    
                }
                catch( const std::exception & e ) {
                    LOG(DEBUG)<<"Exception in receive loop: "<<e.what();
                    size_t retryCount = 0;
                    bool success = false;
                    while(!isClosed.load() && retryCount < maxReconnectCount){
                        LOG(DEBUG)<<"try reconnect...";
                        try{
                            retryCount++;
                            sock.close();
                            sock.connect(sa);
                            onReconnect();
                            success = true;
                            if(sendbuff != ""){
                                send(sendbuff);
                                sendbuff = "";
                            }
                            break;
                        }catch(const std::exception & e){
                            //std::cout<<"Error in reconnect: "<<e.what()<<std::endl;
                        }               
                        std::this_thread::sleep_for(std::chrono::milliseconds{500});
                    }
                    if(!success){
                        onClose();
                        break;
                    }else{
                        LOG(DEBUG) << "reconnected tcp client...";
                    }
                }
            }
        }
    } );
}
