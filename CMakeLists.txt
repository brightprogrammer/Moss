# cmake settings
cmake_minimum_required(VERSION 3.5)

# project settings
project(moss VERSION 0 LANGUAGES C CXX)
set(CMAKE_BUILD_TYPE Debug)

# add kernel
add_subdirectory(Kernel)
