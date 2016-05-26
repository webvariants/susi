
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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(libbson REQUIRED)
find_package(mongoc REQUIRED)

# execute_process(COMMAND /bin/sh ${CMAKE_SOURCE_DIR}/components/susi-mongodb/driver-install.sh)

include_directories(${LIBBSON_INCLUDE_DIRS})
include_directories(${MONGOC_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../submodules/mongo-cxx-driver/src)
include_directories(${CMAKE_BINARY_DIR}/install/include/bsoncxx/v_noabi)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../submodules/mongo-cxx-driver/src)

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall --std=c++11 -O3")

file( GLOB SRCFILES ${CMAKE_CURRENT_SOURCE_DIR}/sources/*.cpp )

file( GLOB HEADERFILES ${CMAKE_CURRENT_SOURCE_DIR}/headers/**/*.h )

add_executable( ${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${SRCFILES})

target_link_libraries(${PROJECT_NAME}
	${CMAKE_THREAD_LIBS_INIT}
	${Boost_LIBRARIES}
	${OPENSSL_LIBRARIES}
	${LIBBSON_LIBRARIES}
	${MONGOC_LIBRARIES}
	mongocxx
	bson
	susi-cpp)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "/usr/local/bin")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/headers/ DESTINATION /usr/local/include)
