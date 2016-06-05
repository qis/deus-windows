cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(png VERSION 1.6.21 LANGUAGES C)

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

# Packages
find_package(zlib REQUIRED PATHS ../zlib)

# Configuration Files
configure_file(src/pnglibconf.h.in ${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.h)

# Headers
file(GLOB_RECURSE headers include/*.h)
list(APPEND headers ${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.h)

# Sources
file(GLOB_RECURSE sources src/*.in src/*.h src/*.c)

# Library
add_library(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} PUBLIC zlib)

# Include Directories
target_include_directories(${PROJECT_NAME}
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR} src
  PUBLIC include)

# Install Target
install(TARGETS ${PROJECT_NAME} DESTINATION lib/$<LOWER_CASE:$<CONFIG>>)

# Install Headers
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.h DESTINATION include)
