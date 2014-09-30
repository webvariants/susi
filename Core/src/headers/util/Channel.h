/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Tino Rusch (tino.rusch@webvariants.de)
 */

#ifndef __CHANNEL__
#define __CHANNEL__

#include <deque>
#include <mutex>
#include <condition_variable>

namespace Susi {
    namespace Util {

        class ChannelClosedException: public std::exception {
            virtual const char* what() const throw() {
                return "Channel was closed";
            }
        };

        template<class T>
        class Channel : std::deque<T> {
        protected:
            std::mutex mutex;
            std::condition_variable not_empty;
            std::condition_variable not_full;
            size_t capacity;
            bool closed = false;
        public:

            Channel( size_t cap ) : capacity( cap ) {}
            Channel() : capacity( 1 ) {}
            ~Channel() {
                close();
            }

            void put( T data ) {
                std::unique_lock<std::mutex> lock( mutex );
                not_full.wait( lock,[this]() {
                    return ( size()<capacity || closed );
                } );
                if( closed )throw ChannelClosedException {};
                this->push_back( std::move( data ) );
                not_empty.notify_one();
            }

            T get() {
                std::unique_lock<std::mutex> lock( mutex );
                not_empty.wait( lock,[this]() {
                    return ( size()>0 || closed );
                } );
                if( closed && this->size()==0 )throw ChannelClosedException {};
                T result = std::move( this->front() );
                this->pop_front();
                not_full.notify_one();
                return result;
            }

            size_t size() {
                return std::deque<T>::size();
            }

            size_t cap() {
                return this->capacity;
            }

            void close() {
                std::unique_lock<std::mutex> lock( mutex );
                closed = true;
                not_empty.notify_all();
                not_full.notify_all();
            }

        };

    }
}


#endif // __CHANNEL__