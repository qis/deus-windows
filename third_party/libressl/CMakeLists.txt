cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(libressl VERSION 2.3.5 LANGUAGES C)

# Compiler
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Configurations
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# Compiler Options
foreach(flag
    CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
    CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
  if(${flag} MATCHES "/MD")
    string(REPLACE "/MD" "/MT" ${flag} "${${flag}}")
  endif()
endforeach()

# Definitions
add_definitions(-D_UNICODE -DUNICODDE -DWIN32_LEAN_AND_MEAN -DNOMINMAX)
add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_ATL_SECURE_NO_DEPRECATE)
add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
add_definitions(-DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00)

add_definitions(-w)

# Library
add_subdirectory(src)
