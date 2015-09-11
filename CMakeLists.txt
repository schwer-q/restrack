#
# Copyright (c) 2015, Quentin Schwerkolt
# All rights reserved.
#

cmake_minimum_required (VERSION 2.8)
project (Restrack)

set(CLANG_INCLUDEDIR "/usr/include" CACHE STRING "user input")
set(CLANG_LIBDIR "/usr/lib" CACHE STRING "user input")

include_directories("${CLANG_INCLUDEDIR}")
link_directories("${CLANG_LIBDIR}")

add_executable(restrack restrack.c)
target_link_libraries(restrack clang)
