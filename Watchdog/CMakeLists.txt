###################################################################################
#
#  Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
#
#  This file is released under the terms of the MIT license. You can find the
#  complete text in the attached LICENSE file or online at:
#
#  http://www.opensource.org/licenses/mit-license.php
#
#  @author: Thomas Krause (thomas.krause@webvariants.de)
#
###################################################################################

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project( "WATCHDOG" )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/dev")

find_package(POCO REQUIRED PocoFoundation)
find_package(Threads REQUIRED)

#add_subdirectory( ${CMAKE_SOURCE_DIR}/test/gtest-1.7.0 )
#include_directories( ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} )

set( CMAKE_COLOR_MAKEFILE ON )

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -Wall --std=c++11 -fPIC -O0" )

include_directories( ${CMAKE_SOURCE_DIR}/src/headers )
#include_directories( ${CMAKE_SOURCE_DIR}/test/gtest-1.7.0/include )
include_directories( ${POCO_INCLUDE_DIR} )
include_directories( ${SOCI_INCLUDE_DIRS} )

file( GLOB SRCFILES ${CMAKE_SOURCE_DIR}/src/sources/*.cpp )

#WATCHDOG BINARY
add_executable( watchdog ${SRCFILES} ${CMAKE_SOURCE_DIR}/src/sources/main.cc )
target_link_libraries(watchdog ${POCO_LIBRARIES})
target_link_libraries(watchdog ${CMAKE_THREAD_LIBS_INIT})



