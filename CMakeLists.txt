cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(hlc
  LANGUAGES C)

# Set the default C standard:

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Enable compiler warnings:

set(c_compiler "${CMAKE_C_COMPILER_ID}")

if(NOT CMAKE_C_SIMULATE_ID STREQUAL "")
  set(c_compiler "${CMAKE_C_SIMULATE_ID}")
endif()

if(NOT CMAKE_C_COMPILER_FRONTEND_VARIANT STREQUAL "")
  set(c_compiler "${CMAKE_C_COMPILER_FRONTEND_VARIANT}")
endif()

if(c_compiler MATCHES "^(GNU|(ARM|Apple)?Clang|Intel(LLVM)?)$")
  set(c_flags "-pedantic-errors -Wall -Wextra")
elseif(c_compiler STREQUAL "MSVC")
  set(c_flags /Wall)
endif()

if(DEFINED c_flags)
  if(CMAKE_C_FLAGS STREQUAL "")
    set(CMAKE_C_FLAGS "${c_flags}")
  else()
    string(APPEND CMAKE_C_FLAGS " ${c_flags}")
  endif()
endif()

# Add hlc executable:

add_executable(hlc
  avl.c
  layout.c
  map.c
  random.c
  set.c
  traits/assign.c
  traits/compare.c
  traits/delete.c
  main.c)

target_compile_definitions(hlc
  PRIVATE HLC_EXPORTS _CRTDBG_MAP_ALLOC)
