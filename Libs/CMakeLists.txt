cmake_minimum_required(VERSION 3.12)

add_compile_definitions(#ELPP_NO_GLOBAL_LOCK 
                        ELPP_THREAD_SAFE)
                        # ELPP_FORCE_USE_STD_THREAD)
add_library(rov_libs easyloggingpp/src/easylogging++.cc)
