cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(../include)

add_library(rawsocket STATIC
  rawsocket.cpp
  ../include/rawsocket.h
)

set_target_properties(rawsocket PROPERTIES
         ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

