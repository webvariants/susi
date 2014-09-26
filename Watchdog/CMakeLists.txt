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

find_package(POCO REQUIRED PocoFoundation PocoUtil PocoJSON PocoNet)
find_package(SOCI REQUIRED)
find_package(Threads REQUIRED)

#add_subdirectory( ${CMAKE_SOURCE_DIR}/test/gtest-1.7.0 )
#include_directories( ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} )

set( CMAKE_COLOR_MAKEFILE ON )

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -Wall --std=c++11 -fPIC -O0" )

include_directories( ${CMAKE_SOURCE_DIR}/src/headers )
#include_directories( ${CMAKE_SOURCE_DIR}/test/gtest-1.7.0/include )
include_directories( ${POCO_INCLUDE_DIR} )
include_directories( ${SOCI_INCLUDE_DIRS} )

file( GLOB SRCFILES ${CMAKE_SOURCE_DIR}/src/sources/**/*.cpp )
#file( GLOB TESTFILES ${CMAKE_SOURCE_DIR}/test/*/*.cpp )

#LIBRARY
add_library( libwatchdog SHARED ${SRCFILES} )
set_target_properties(libwatchdog PROPERTIES OUTPUT_NAME watchdog)

#WATCHDOG BINARY
add_executable( watchdog ${CMAKE_SOURCE_DIR}/src/sources/main.cc )
target_link_libraries(watchdog libwatchdog)
target_link_libraries(watchdog ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(watchdog ${SOCI_LIBRARY})
target_link_libraries(watchdog ${POCO_LIBRARIES})

#SUSI TESTS
#add_executable( susi_test ${TESTFILES})
#target_link_libraries(susi_test libsusi)
#target_link_libraries(susi_test ${CMAKE_THREAD_LIBS_INIT})
#target_link_libraries(susi_test ${SOCI_LIBRARY})
#target_link_libraries(susi_test ${POCO_LIBRARIES})
#target_link_libraries(susi_test gtest)
#target_link_libraries(susi_test gtest_main)


