cmake_minimum_required(VERSION 3.10)
project(protocol)



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)




include_directories(${CMAKE_SOURCE_DIR}/include)


file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/cpp/*.cpp)


add_executable(protocol ${SRC_FILES})