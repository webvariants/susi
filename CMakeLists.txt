
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

set(CMAKE_BUILD_TYPE Debug)
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

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/susi-common)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/susi-common/headers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/susi-core)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/susi-core/headers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/susi-cpp)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/susi-cpp/headers)
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/susi-util)
# include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/susi-util/headers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/susi-leveldb)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/susi-leveldb/headers)
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/susi-http2)
# include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/susi-http2/headers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/susi-duktape)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/susi-duktape/headers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/susi-cluster)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/susi-cluster/headers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/susi-udpserver)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/susi-udpserver/headers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/susi-heartbeat)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/susi-heartbeat/headers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/susi-mqtt)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/susi-mqtt/headers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/susi-webhooks)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/susi-webhooks/headers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/susi-authenticator)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/susi-authenticator/headers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/susi-serial)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/susi-serial/headers)

SET(CPACK_GENERATOR "DEB")
SET(CPACK_SET_DESTDIR ON)
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Tino Rusch")
SET(CPACK_PACKAGE_VERSION "0.1.7")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "7")
INCLUDE(CPack)
