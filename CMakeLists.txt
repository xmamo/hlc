cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(hlc LANGUAGES C)

include(CheckSymbolExists)

add_executable(
  hlc
  avl.c
  layout.c
  map.c
  set.c
  traits/assign.c
  traits/compare.c
  traits/delete.c
  main.c
)

target_compile_features(hlc PRIVATE c_std_17)
target_compile_options(hlc PRIVATE -Wall -Wextra -pedantic-errors)
target_compile_definitions(hlc PRIVATE HLC_EXPORTS)
