cmake_minimum_required(VERSION 3.10)
project(mpi_test)
include_directories(/usr/local/mpich/include)

set(CMAKE_CXX_STANDARD 11)

add_executable(mpi_test main_back.cpp)