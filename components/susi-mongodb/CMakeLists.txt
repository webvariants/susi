
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

project(susi-mongodb)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/headers )

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall --std=c++11 -O3 \
 -I/usr/local/include/mongocxx/v_noabi \
 -I/usr/local/include/libmongoc-1.0 \
 -I/usr/local/include/bsoncxx/v_noabi \
 -I/usr/local/include/libbson-1.0" )

file( GLOB SRCFILES ${CMAKE_CURRENT_SOURCE_DIR}/sources/*.cpp )

file( GLOB HEADERFILES ${CMAKE_CURRENT_SOURCE_DIR}/headers/**/*.h )

add_executable( ${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${SRCFILES})

target_link_libraries(${PROJECT_NAME}
	${CMAKE_THREAD_LIBS_INIT}
	${Boost_LIBRARIES}
	${OPENSSL_LIBRARIES}
	bson
	susi-cpp
	mongocxx
	bsoncxx)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "/usr/local/bin")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/headers/ DESTINATION /usr/local/include)
