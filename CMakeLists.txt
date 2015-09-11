#
# Copyright (c) 2015, Quentin Schwerkolt
# All rights reserved.
#

cmake_minimum_required (VERSION 2.6)
project (Restrack)

add_executable(restrack restrack.c)
target_link_libraries(restrack clang)
