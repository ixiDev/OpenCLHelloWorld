cmake_minimum_required(VERSION 3.29)
project(HelloWorld)

set(CMAKE_CXX_STANDARD 14)

# OpenCL Config
find_package(OpenCL REQUIRED)
include_directories( ${OPENCL_INCLUDE_DIR} )
include_directories(/usr/local/include)

add_executable(HelloWorld main.cpp)

target_link_libraries(HelloWorld OpenCL::OpenCL)