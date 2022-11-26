cmake_minimum_required(VERSION 3.5)

add_executable(test_jy901 test_jy901.cpp)
target_link_libraries(test_jy901 rov_driver)


add_executable(test_ms5837 test_ms5837.cpp)
target_link_libraries(test_ms5837 rov_driver)

