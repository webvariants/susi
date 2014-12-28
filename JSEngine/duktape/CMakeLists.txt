###################################################################################
#
#  Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
#
#  This file is released under the terms of the MIT license. You can find the
#  complete text in the attached LICENSE file or online at:
#
#  http://www.opensource.org/licenses/mit-license.php
#
#  @author: Tino Rusch (tino.rusch@webvariants.de), Christian Sonderfeld (christian.sonderfeld@webvariants.de)
#
###################################################################################

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project( "SUSI-CPPENGINE" )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/dev")

find_package(POCO REQUIRED PocoFoundation PocoUtil PocoJSON PocoNet)
find_package(SOCI REQUIRED)
find_package(LibLZMA REQUIRED)
find_package(Threads REQUIRED)

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -Wall --std=c++11 -fPIC -O3" )

include_directories( ${CMAKE_SOURCE_DIR}/src/headers )
include_directories( ${CMAKE_SOURCE_DIR}/../../Core/src/headers )
include_directories( ${POCO_INCLUDE_DIR} )
include_directories( ${SOCI_INCLUDE_DIRS} )

file( GLOB SRCFILES ${CMAKE_SOURCE_DIR}/src/sources/*.cpp )

#easylogging++ defines
add_definitions(-D_ELPP_THREAD_SAFE)

add_executable( susi-jsengine ${CMAKE_SOURCE_DIR}/src/main.cc ${SRCFILES})

target_link_libraries(susi-jsengine ${CMAKE_SOURCE_DIR}/../../bin/libsusi.so)
target_link_libraries(susi-jsengine ${POCO_LIBRARIES})
target_link_libraries(susi-jsengine ${LIBLZMA_LIBRARIES})
target_link_libraries(susi-jsengine ${SOCI_LIBRARY})
target_link_libraries(susi-jsengine ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(susi-jsengine tecla_r)
target_link_libraries(susi-jsengine ncurses)
target_link_libraries(susi-jsengine onion)
target_link_libraries(susi-jsengine onioncpp)
