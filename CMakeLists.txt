cmake_minimum_required(VERSION 3.3)
project(Packet)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -Wall -Werror")

set(SOURCE_FILES packet.h)
# add_executable(Packet ${SOURCE_FILES})

add_subdirectory(lib/gtest)
enable_testing()

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(packet_tests test_packet.cpp)
target_link_libraries(packet_tests gtest gtest_main)

add_test(NAME Packet-Test COMMAND packet_tests)

set(PACKET_HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PACKET_LIBRARY_PATH ${PROJECT_BINARY_DIR})
