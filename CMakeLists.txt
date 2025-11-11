cmake_minimum_required(VERSION 3.31)
project(ini C)

set(CMAKE_C_STANDARD 11)

option(INI_TEST "Enable INI testing mode" OFF)

add_library(ini STATIC
        ini.c
        ini.h)
target_include_directories(ini PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(INI_TEST)
    add_compile_definitions(INI_TEST)
    add_executable(ini_tests
            tests/rktest.c
            tests/ini_tests.c
    )
    target_link_libraries(ini_tests PRIVATE ini m)
    target_compile_options(ini_tests PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(ini PRIVATE -Wall -Wextra -pedantic)
    target_compile_definitions(ini PRIVATE INI_TEST)
    target_compile_definitions(ini_tests PRIVATE INI_TEST)
    target_include_directories(ini PRIVATE src)
endif()
