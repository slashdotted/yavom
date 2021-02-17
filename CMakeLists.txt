cmake_minimum_required(VERSION 3.5)
project(yavom LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Wpedantic -Werror")
set(CMAKE_C_FLAGS_DEBUG "-g -Wall -Wextra -Wpedantic -Werror")

add_executable(yavom test.cpp diff.h)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/testdata/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/testdata/)

option(USE_TRANSPOSITION "(Experimental) Allow for swapping source and target during computation" OFF) #OFF by default
if(USE_TRANSPOSITION)
    target_compile_definitions(yavom PRIVATE YAVOM_TRANSPOSE="1")
endif(USE_TRANSPOSITION)

option(USE_MULTITHREAD "(Experimental) Use separate async for forward/backward steps" OFF) #OFF by default
if(USE_ASYNC)
    find_package (Threads)
    target_link_libraries (yavom ${CMAKE_THREAD_LIBS_INIT})
    target_compile_definitions(yavom PRIVATE YAVOM_ASYNC="1")
endif(USE_ASYNC)
#unset(USE_ASYNC CACHE)
