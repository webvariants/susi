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

project( "SUSI" )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/dev")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(POCO REQUIRED PocoFoundation PocoUtil PocoNet)
find_package(SOCI REQUIRED)
find_package(LibLZMA REQUIRED)
find_package(Threads REQUIRED)

add_subdirectory( ${CMAKE_SOURCE_DIR}/src/plugins )

add_subdirectory( ${CMAKE_SOURCE_DIR}/test/gtest-1.7.0 )
include_directories( ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} )

set( CMAKE_COLOR_MAKEFILE ON )

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -Wall --std=c++11 -fPIC -O0" )

include_directories( ${CMAKE_SOURCE_DIR}/src/headers )
include_directories( ${CMAKE_SOURCE_DIR}/test/gtest-1.7.0/include )
include_directories( ${POCO_INCLUDE_DIR} )
include_directories( ${SOCI_INCLUDE_DIRS} )

file( GLOB SRCFILES ${CMAKE_SOURCE_DIR}/src/sources/**/*.cpp )
file( GLOB TESTFILES ${CMAKE_SOURCE_DIR}/test/*/*.cpp )

#easylogging++ defines 
add_definitions(-D_ELPP_THREAD_SAFE)

#LIBRARY
add_library( libsusi SHARED ${SRCFILES} )
set_target_properties(libsusi PROPERTIES OUTPUT_NAME susi)

#SUSI BINARY
add_executable( susi ${CMAKE_SOURCE_DIR}/src/sources/main.cc )
target_link_libraries(susi libsusi)
target_link_libraries(susi ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(susi ${SOCI_LIBRARY})
target_link_libraries(susi ${LIBLZMA_LIBRARIES})
target_link_libraries(susi ${POCO_LIBRARIES})
# target_link_libraries(susi PocoFoundation PocoUtil PocoNet)

#SUSI TESTS
add_executable( susi_test ${CMAKE_SOURCE_DIR}/test/easylogging++_init.cpp ${TESTFILES})
target_link_libraries(susi_test libsusi)
target_link_libraries(susi_test ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(susi_test ${SOCI_LIBRARY})
target_link_libraries(susi_test ${LIBLZMA_LIBRARIES})
target_link_libraries(susi_test ${POCO_LIBRARIES})
target_link_libraries(susi_test gtest)
target_link_libraries(susi_test gtest_main)


