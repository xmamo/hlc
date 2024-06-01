cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(hlc LANGUAGES C)

include(CheckSymbolExists)
check_symbol_exists(alloca stdlib.h HLC_HAVE_ALLOCA)

add_executable(hlc traits/assign.c traits/compare.c avl.c set.c main.c)
target_compile_features(hlc PRIVATE c_std_17)
target_compile_options(hlc PRIVATE -Wall -Wextra)

target_compile_definitions(hlc PRIVATE HLC_EXPORTS)

if(HLC_HAVE_ALLOCA)
  target_compile_definitions(hlc PRIVATE HLC_HAVE_ALLOCA)
endif()
