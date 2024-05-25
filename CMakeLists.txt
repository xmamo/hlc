cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(hlc LANGUAGES C)

add_executable(hlc main.c avl.c set.c)
target_compile_features(hlc PRIVATE c_std_17)
target_compile_options(hlc PRIVATE -Wall -Wextra)
