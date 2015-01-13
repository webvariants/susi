cmake_minimum_required(VERSION 2.8)

project (Heartbeat)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "--std=c++11 -g ${CMAKE_CXX_FLAGS}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/headers)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../headers)

add_library(heartbeat SHARED sources/export.cpp)
