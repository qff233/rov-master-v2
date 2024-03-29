cmake_minimum_required(VERSION 3.12)
project(rov_master_v2)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/Libs)

include_directories(.)
include_directories(Drivers)
include_directories(Libs/spdlog/include)
include_directories(Libs/wiringNP/wiringPi)
include_directories(Libs/json-rpc-cxx/include)
include_directories(Libs/json-rpc-cxx/vendor)

add_subdirectory(Libs)
add_subdirectory(Drivers)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -w -DNDEBUG")

aux_source_directory(./User user_files)
aux_source_directory(./User/Event user_event_files)
aux_source_directory(./User/Control user_control_files)
aux_source_directory(./User/Connection user_connection_files)

add_library(rov_user ${user_files} 
                     ${user_event_files} 
                     ${user_control_files} 
                     ${user_connection_files})

add_executable(rovmaster ${user_files})
target_link_libraries(rovmaster rov_user 
                                 rov_driver 
                                 pthread)

#add_subdirectory(Test)

