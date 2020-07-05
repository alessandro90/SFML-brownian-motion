cmake_minimum_required (VERSION 3.10.0)

project(SFML_PLAY)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -O2")

if (WIN32)
    message("Compiling in a windows environment")
    set(COMPILER_DEFINITIONS -mwindows)
endif()

add_definitions(${COMPILER_DEFINITIONS})

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(brownian brownian.cpp)
target_link_libraries(brownian sfml-window sfml-graphics sfml-system pthread)
