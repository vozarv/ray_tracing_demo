cmake_minimum_required(VERSION 3.26)

project(ray_tracing_demo
        VERSION 1.0
        LANGUAGES CXX
)

# Add the -pg flag for profiling
# for profiling : gprof ray_tracing gmon.out > analysis.txt

# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
# endif()


add_executable(ray_tracing main.cpp)

add_executable(business_card card.cpp)