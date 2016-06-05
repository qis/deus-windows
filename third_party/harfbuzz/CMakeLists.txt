cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(harfbuzz VERSION 1.2.7 LANGUAGES CXX)

# Compiler
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

add_definitions(-DHAVE_CONFIG_H -DHB_NO_MT -w)

# Packages
#find_package(zlib REQUIRED PATHS ../zlib)
#find_package(png  REQUIRED PATHS ../png)

# Sources
file(GLOB_RECURSE sources src/*.in src/*.h src/*.c)

set(sources
  src/hb-ucdn/ucdn.c
  src/hb-ucdn.cc
  src/hb-blob.cc
  src/hb-buffer.cc
  src/hb-common.cc
  src/hb-face.cc
  src/hb-font.cc
  src/hb-set.cc
  src/hb-shape.cc
  src/hb-shape-plan.cc
  src/hb-shaper.cc
  src/hb-unicode.cc
  src/hb-warning.cc
  src/hb-ot-tag.cc
  src/hb-ot-font.cc
  src/hb-ot-layout.cc
  src/hb-ot-map.cc
  src/hb-ot-shape.cc
  src/hb-ot-shape-complex-arabic.cc
  src/hb-ot-shape-complex-default.cc
  src/hb-ot-shape-complex-hangul.cc
  src/hb-ot-shape-complex-hebrew.cc
  src/hb-ot-shape-complex-indic.cc
  src/hb-ot-shape-complex-indic-table.cc
  src/hb-ot-shape-complex-myanmar.cc
  src/hb-ot-shape-complex-thai.cc
  src/hb-ot-shape-complex-tibetan.cc
  src/hb-ot-shape-complex-use.cc
  src/hb-ot-shape-complex-use-table.cc
  src/hb-ot-shape-normalize.cc
  src/hb-ot-shape-fallback.cc)

# Library
add_library(${PROJECT_NAME} ${sources})
#target_link_libraries(${PROJECT_NAME} PUBLIC zlib png)

# Include Directories
target_include_directories(${PROJECT_NAME}
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR} src src/hb-ucdn src/generated ../freetype/include
  PUBLIC include)

# Install Target
install(TARGETS ${PROJECT_NAME} DESTINATION lib/$<LOWER_CASE:$<CONFIG>>)

# Install Headers
install(DIRECTORY src/include/ DESTINATION include FILES_MATCHING PATTERN "*.h")
