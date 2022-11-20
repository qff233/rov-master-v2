cmake_minimum_required(VERSION 3.0)
project(rov_master_se)

set(CMAKE_CXX_STANDARD 11)

add_executable(rov_master_se main.cpp Drivers/jy901.cpp Drivers/jy901.h config.h Drivers/ms5837.cpp Drivers/ms5837.h)
