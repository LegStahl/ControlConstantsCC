cmake_minimum_required(VERSION 3.10)
project(protocol)


foreach(OUTPUTCONFIG Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} 
        ${CMAKE_SOURCE_DIR}/bin/${OUTPUTCONFIG})
endforeach()


include_directories(${CMAKE_SOURCE_DIR}/include)


file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/cpp/*.cpp)


add_executable(protocol ${SRC_FILES})