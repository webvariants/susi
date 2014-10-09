#include "util/ThreadPool.h"

Susi::Util::ThreadPool::ThreadPool( size_t workers, size_t buffsize ) : _workChannel {buffsize} {
    for( size_t i=0; ( i<workers ); ++i ) {
        startThread();
    }
}

void Susi::Util::ThreadPool::startThread() {
    std::thread t {[this]() {
        while( true ) {
            Work work;
            try {
                work = this->_workChannel.get();
            }
            catch( const std::exception & e ) {
                //std::cout<<"error in worker: "<<e.what()<<std::endl;
                return;
            }
            try {
                work.work();
            }
            catch( const std::exception & e ) {
                if( work.error ) {
                    work.error( e.what() );
                }
            }
        }
    }};
    _threads.push_back( std::move( t ) );
}

void Susi::Util::ThreadPool::add( std::function<void()> work,
                                  std::function<void( std::string )> error ) {
    _workChannel.put( Work {work,error} );
}

Susi::Util::ThreadPool::~ThreadPool() {
    _workChannel.close();
    for( auto & t : _threads ) {
        t.join();
    }
}