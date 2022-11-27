cmake_minimum_required(VERSION 3.5)
project(rov_master_v2)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/Libs)

include_directories(Drivers)
include_directories(Libs/easylogger/inc)
include_directories(.)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_FLAGS_DEBUG  "-O0 -g -Wall")
set(CMAKE_C_FLAGS_RELEASE "-O3 -w")
set(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -w")

# add_executable(rov_master_se main.cpp Drivers/jy901.cpp Drivers/jy901.h config.h Drivers/ms5837.cpp Drivers/ms5837.h)
add_subdirectory(Drivers)
add_subdirectory(Test)

