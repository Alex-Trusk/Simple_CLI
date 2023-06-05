cmake_minimum_required(VERSION 3.0.0)
project(Simple_CLI VERSION 0.1.0)

if(NOT CMAKE_C_FLAGS)
	set(CMAKE_C_FLAGS "-O0 -std=gnu99")
endif()
if(NOT CMAKE_C_FLAGS_DEBUG)
	set(CMAKE_C_FLAGS_DEBUG "-g -ggdb")
endif()
add_compile_options(
    -Werror

    -Wall
    -Wextra
    -Wpedantic

    -Wcast-align
    -Wcast-qual
#    -Wconversion
    -Wenum-compare
    -Wfloat-equal
    -Wredundant-decls
    -Wsign-conversion
)

#include(CTest)

include_directories(Lib)
enable_testing()
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(Lib)
add_library(command_set STATIC ./Test/cli_command_set.c)
link_libraries(simple_cli command_set)
add_executable(Simple_CLI ./Test/main.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
