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

project( "SUSI-DEBUGGER" )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/dev")

find_package(POCO REQUIRED PocoFoundation PocoUtil PocoJSON PocoNet)
find_package(SOCI REQUIRED)
find_package(Threads REQUIRED)

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -Wall --std=c++11 -fPIC -O3" )

# ADD_SUBDIRECTORY (${CMAKE_SOURCE_DIR}/test/gtest-1.7.0)
# include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

include_directories( ${CMAKE_SOURCE_DIR}/../Core/src/headers )
include_directories( ${CMAKE_SOURCE_DIR}/../CPPEngine/src/headers )
include_directories( ${CMAKE_SOURCE_DIR}/src/headers )
include_directories( ${POCO_INCLUDE_DIR} )
include_directories( ${SOCI_INCLUDE_DIRS} )

file( GLOB SRCFILES ${CMAKE_SOURCE_DIR}/src/sources/*.cpp )

add_executable( susi-debugger ${SRCFILES} ${CMAKE_SOURCE_DIR}/src/sources/main.cc)
target_link_libraries(susi-debugger ${CMAKE_SOURCE_DIR}/../CPPEngine/build/libsusicpp.so)
target_link_libraries(susi-debugger ${CMAKE_SOURCE_DIR}/../Core/build/libsusi.so)
target_link_libraries(susi-debugger ${POCO_LIBRARIES})
target_link_libraries(susi-debugger ${SOCI_LIBRARY})
target_link_libraries(susi-debugger ${CMAKE_THREAD_LIBS_INIT})


# file( GLOB TESTFILES ${CMAKE_SOURCE_DIR}/test/*.cpp )

# add_executable( susi_cppengine_test ${SRCFILES} ${TESTFILES} )
# target_link_libraries(susi_cppengine_test ${CMAKE_SOURCE_DIR}/../Core/build/libsusi.so)
# target_link_libraries(susi_cppengine_test ${POCO_LIBRARIES})
# target_link_libraries(susi_cppengine_test ${CMAKE_THREAD_LIBS_INIT})
# target_link_libraries(susi_cppengine_test gtest)
# target_link_libraries(susi_cppengine_test gtest_main)
