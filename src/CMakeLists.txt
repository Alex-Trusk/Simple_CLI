cmake_minimum_required(VERSION 3.0.0)
get_directory_property(IS_SUBPROJECT PARENT_DIRECTORY)
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

set(LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/lib)
add_library(simple_cli STATIC simple_cli.c)