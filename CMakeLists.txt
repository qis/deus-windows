cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(DEUS VERSION 0.1.0 LANGUAGES CXX)
set(PRODUCT_NAME "DEUS Client")
set(COPYING_TEXT "2016 Alexej Harm")

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

# Linker Options
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /manifestuac:NO /ignore:4099")

# Dependencies
find_program(PANDOC_PATH REQUIRED NAMES pandoc)
if(NOT PANDOC_PATH)
  message(FATAL_ERROR "Could not find program: pandoc")
endif()

find_program(7ZIP_PATH REQUIRED NAMES 7z PATHS "C:/Program Files/7-Zip")
if(NOT 7ZIP_PATH)
  message(FATAL_ERROR "Could not find program: 7z")
endif()

# Packages
find_package(compat   REQUIRED PATHS third_party/compat)
find_package(angle    REQUIRED PATHS third_party/angle)
find_package(asio     REQUIRED PATHS third_party/asio)
find_package(jpeg     REQUIRED PATHS third_party/jpeg)
find_package(png      REQUIRED PATHS third_party/png)
find_package(freetype REQUIRED PATHS third_party/freetype)
find_package(harfbuzz REQUIRED PATHS third_party/harfbuzz)
find_package(zip      REQUIRED PATHS third_party/zip)

# Functions
function(assign_source_group)
  foreach(filename IN ITEMS ${ARGN})
    file(RELATIVE_PATH source "${CMAKE_CURRENT_SOURCE_DIR}" "${filename}")
    get_filename_component(source_path "${source}" PATH)
    if(MSVC)
      string(REPLACE "/" "\\" source_path "${source_path}")
    endif()
    source_group("${source_path}" FILES "${filename}")
  endforeach()
endfunction(assign_source_group)

# Configuration Files
configure_file(res/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
configure_file(res/config.user ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.vcxproj.user)

# Documentation
file(GLOB doc doc/*.md)
assign_source_group(${doc})

# Resources
file(GLOB_RECURSE res res/*)
assign_source_group(${res})

# Sources
file(GLOB_RECURSE src src/*.h src/*.cc)
assign_source_group(${src})

# Executable
add_executable(${PROJECT_NAME} WIN32 ${doc} ${res} ${src} readme.md)
target_link_libraries(${PROJECT_NAME} PRIVATE compat GLESv2 EGL asio_ssl jpeg png freetype harfbuzz zip)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME $<LOWER_CASE:${PROJECT_NAME}>)

# Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} src res)

# Install Targets
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Install Libraries
install(FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/angle/lib/$<LOWER_CASE:$<CONFIG>>/libEGL.dll
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/angle/lib/$<LOWER_CASE:$<CONFIG>>/libGLESv2.dll
  DESTINATION bin)

# Install Resources
set(7ZIP_OPTIONS "a -tzip \"${CMAKE_CURRENT_BINARY_DIR}/data.pak\" \"*\"")
set(7ZIP_COMMAND "\"${7ZIP_PATH}\" ${7ZIP_OPTIONS} WORKING_DIRECTORY \"${CMAKE_CURRENT_SOURCE_DIR}/res/data\"")
install(CODE "execute_process(COMMAND ${7ZIP_COMMAND})")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/data.pak DESTINATION bin)


# Install Documentation
file(GLOB PANDOC_HTML res/pandoc.html)
foreach(src IN ITEMS ${doc})
  get_filename_component(name "${src}" NAME_WE)
  set(dst "${CMAKE_CURRENT_BINARY_DIR}/${name}.html")
  set(PANDOC_OPTIONS "-s -S -f markdown -t html -H \"${PANDOC_HTML}\"")
  set(PANDOC_COMMAND "\"${PANDOC_PATH}\" ${PANDOC_OPTIONS} -o \"${dst}\" \"${src}\"")
  install(CODE "execute_process(COMMAND ${PANDOC_COMMAND})")
  install(FILES ${dst} DESTINATION bin)
endforeach()
