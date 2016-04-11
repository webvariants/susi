
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

project(susi-leveldb)

# Find libleveldb.a - key/value storage system
find_path(LevelDB_INCLUDE_PATH NAMES leveldb/db.h)
find_library(LevelDB_LIBRARY NAMES libleveldb.a libleveldb.lib)

if(LevelDB_INCLUDE_PATH AND LevelDB_LIBRARY)
  set(LevelDB_FOUND TRUE)
endif(LevelDB_INCLUDE_PATH AND LevelDB_LIBRARY)

if(LevelDB_FOUND)
  if(NOT LevelDB_FIND_QUIETLY)
    message(STATUS "Found LevelDB: ${LevelDB_LIBRARY}")
  endif(NOT LevelDB_FIND_QUIETLY)
else(LevelDB_FOUND)
  if(LevelDB_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find leveldb library.")
  endif(LevelDB_FIND_REQUIRED)
endif(LevelDB_FOUND)

include_directories( ${LevelDB_INCLUDE_PATH} )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/headers )

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall --std=c++11 -Os " )
file( GLOB SRCFILES ${CMAKE_CURRENT_SOURCE_DIR}/sources/SusiLevelDBClient.cpp)
file( GLOB HEADERFILES ${CMAKE_CURRENT_SOURCE_DIR}/headers/*.h )
add_executable( ${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${SRCFILES})
target_link_libraries(${PROJECT_NAME}
	${CMAKE_THREAD_LIBS_INIT}
	${Boost_LIBRARIES}
	${OPENSSL_LIBRARIES}
	${LevelDB_LIBRARY}
	bson
	susi-cpp)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "/usr/local/bin")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/headers/ DESTINATION /usr/local/include)
