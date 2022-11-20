cmake_minimum_required(VERSION 3.5)
project(rov_driver)

file(GLOB source_files "./*.c" "./*.cpp")
add_library(rov_driver ${source_files})
target_link_libraries(rov_driver 
                        wiringPi 
                        easylogger
			pthread)
                        
