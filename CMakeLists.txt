cmake_minimum_required(VERSION 3.5)
project(yavom LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(yavom test.cpp diff.h)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/testdata/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/testdata/)
