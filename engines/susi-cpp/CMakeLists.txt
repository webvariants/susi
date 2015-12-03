
###################################################################################
#
#  Copyright (c) 2015, webvariants GmbH, http://www.webvariants.de
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

project(susi-cpp)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/headers )

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall --std=c++11" )

file( GLOB SUSICPP_SRCFILES ${CMAKE_CURRENT_SOURCE_DIR}/sources/TCPClient.cpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/sources/SSLClient.cpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/sources/SusiClient.cpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/sources/EventManager.cpp)

file( GLOB SUSICOMMON_SRCFILES ${CMAKE_CURRENT_SOURCE_DIR}sources/JSONFramer.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/sources/LineFramer.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/sources/base64.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/sources/sha3.cpp )


add_library( susi-common SHARED ${SUSICOMMON_SRCFILES})
target_link_libraries( susi-common
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    bson)

add_library( ${PROJECT_NAME} SHARED ${SUSICPP_SRCFILES})
target_link_libraries( ${PROJECT_NAME}
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    bson
    susi-common)

add_executable( ${PROJECT_NAME}_test ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp)
target_link_libraries(${PROJECT_NAME}_test
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    bson
    ${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION "/usr/local/lib")
install(TARGETS susi-common LIBRARY DESTINATION "/usr/local/lib")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/headers/ DESTINATION /usr/local/include)
