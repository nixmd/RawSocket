cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(../include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(receiver receiver.cpp)
add_executable(sender sender.cpp)

target_link_libraries(sender rawsocket)
target_link_libraries(receiver rawsocket)

