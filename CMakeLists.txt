
###################################################################################
#
#  Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
#
#  This file is released under the terms of the MIT license. You can find the
#  complete text in the attached LICENSE file or online at:
#
#  http://www.opensource.org/licenses/mit-license.php
#
#  @author: Tino Rusch (tino.rusch@webvariants.de)
#
###################################################################################

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(susi)

option(WITH_CLUSTER "Include susi-cluster binary" ON)
option(WITH_DUKTAPE "Include susi-duktape binary for javascript support" ON)
option(WITH_HEARTBEAT "Include susi-heartbeat binary" ON)
option(WITH_HTTP2 "Include susi-http2 binary'" OFF)
option(WITH_LEVELDB "Include susi-leveldb binary" ON)
option(WITH_MQTT "Include susi-mqtt binary" ON)
option(WITH_SERIAL "Include susi-serial binary" ON)
option(WITH_UDPSERVER "Include susi-udpserver binary" ON)
option(WITH_WEBHOOKS "Include susi-webhooks binary" ON)
option(WITH_STATEFILE "Include susi-webhooks binary" ON)
option(WITH_AUTHENTICATOR "Include susi-authenticator binary" ON)
option(WITH_SHELL "Include susi-shell binary" ON)
option(WITH_OPCUA "Include susi-opcuaserver and susi-opcuaclient binary" ON)
option(WITH_MONGODB "Include susi-mongodb binary" OFF)

set( CMAKE_COLOR_MAKEFILE ON )

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system program_options)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#submodules
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/libbson)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/submodules/libbson/src/headers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/libbcrypt)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/submodules/libbcrypt/include)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/engines/susi-cpp)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/engines/susi-cpp/headers)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/components/susi-core)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/components/susi-core/headers)

if(WITH_CLUSTER)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/components/susi-cluster)
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/components/susi-cluster/headers)
endif(WITH_CLUSTER)

if(WITH_DUKTAPE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/components/susi-duktape)
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/components/susi-duktape/headers)
endif(WITH_DUKTAPE)

if(WITH_HEARTBEAT)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/components/susi-heartbeat)
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/components/susi-heartbeat/headers)
endif(WITH_HEARTBEAT)

if(WITH_HTTP2)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/components/susi-http2)
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/components/susi-http2/headers)
endif(WITH_HTTP2)

if(WITH_LEVELDB)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/components/susi-leveldb)
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/components/susi-leveldb/headers)
endif(WITH_LEVELDB)

if(WITH_MQTT)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/components/susi-mqtt)
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/components/susi-mqtt/headers)
endif(WITH_MQTT)

if(WITH_SERIAL)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/components/susi-serial)
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/components/susi-serial/headers)
endif(WITH_SERIAL)

if(WITH_UDPSERVER)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/components/susi-udpserver)
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/components/susi-udpserver/headers)
endif(WITH_UDPSERVER)

if(WITH_WEBHOOKS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/components/susi-webhooks)
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/components/susi-webhooks/headers)
endif(WITH_WEBHOOKS)

if(WITH_STATEFILE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/components/susi-statefile)
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/components/susi-statefile/headers)
endif(WITH_STATEFILE)

if(WITH_AUTHENTICATOR)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/components/susi-authenticator)
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/components/susi-authenticator/headers)
endif(WITH_AUTHENTICATOR)

if(WITH_SHELL)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/components/susi-shell)
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/components/susi-shell/headers)
endif(WITH_SHELL)

if(WITH_MONGODB)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/components/susi-mongodb)
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/components/susi-mongodb/headers)
endif(WITH_MONGODB)

if(WITH_OPCUA)
    # open62541
    set(UA_ENABLE_SUBSCRIPTIONS ON CACHE BOOL "Enable subscriptions" FORCE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/open62541)
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/submodules/open62541/include)
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/submodules/open62541/plugins)
    include_directories( ${CMAKE_CURRENT_BINARY_DIR}/submodules/open62541/src_generated)
    # susi-opcuaclient and susi-opcuaserver
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/components/susi-opcuaclient)
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/components/susi-opcuaclient/headers)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/components/susi-opcuaserver)
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/components/susi-opcuaserver/headers)
endif(WITH_OPCUA)

if(WITH_OPCUASERVER)
endif(WITH_OPCUASERVER)

SET(CPACK_GENERATOR "DEB")
SET(CPACK_SET_DESTDIR ON)
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Tino Rusch")
SET(CPACK_PACKAGE_VERSION "0.2.0")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "2")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
INCLUDE(CPack)
