cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(jpeg VERSION 1.4.90 LANGUAGES C)

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

# Options
option(WITH_SIMD "Include SIMD extensions" TRUE)
option(WITH_ARITH_ENC "Include arithmetic encoding support" TRUE)
option(WITH_ARITH_DEC "Include arithmetic decoding support" TRUE)
option(WITH_MEM_SRCDST "Include in-memory source/destination manager functions" TRUE)

if(WITH_ARITH_ENC)
  set(C_ARITH_CODING_SUPPORTED 1)
endif()

if(WITH_ARITH_DEC)
  set(D_ARITH_CODING_SUPPORTED 1)
endif()

set(SO_AGE 0)
if(WITH_MEM_SRCDST)
  set(SO_AGE 1)
endif()

if(WITH_MEM_SRCDST)
  set(MEM_SRCDST_SUPPORTED 1)
endif()

set(BITS_IN_JSAMPLE 8)
set(JPEG_LIB_VERSION 80)
set(SIMD_X86_64 1)
set(64BIT 1)

# Configuration Files
configure_file(src/jconfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/jconfig.h)
configure_file(src/jconfigint.h.in ${CMAKE_CURRENT_BINARY_DIR}/jconfigint.h)

# Headers
set(headers
  ${CMAKE_CURRENT_BINARY_DIR}/jconfig.h
  include/jerror.h include/jmorecfg.h include/jpeglib.h)

# Sources
set(sources
  ${CMAKE_CURRENT_BINARY_DIR}/jconfigint.h
  src/jcapimin.c src/jcapistd.c src/jccoefct.c src/jccolor.c  src/jcdctmgr.c src/jchuff.c
  src/jcinit.c   src/jcmainct.c src/jcmarker.c src/jcmaster.c src/jcomapi.c  src/jcparam.c  src/jcphuff.c
  src/jcprepct.c src/jcsample.c src/jctrans.c  src/jdapimin.c src/jdapistd.c src/jdatadst.c src/jdatasrc.c
  src/jdcoefct.c src/jdcolor.c  src/jddctmgr.c src/jdhuff.c   src/jdinput.c  src/jdmainct.c src/jdmarker.c
  src/jdmaster.c src/jdmerge.c  src/jdphuff.c  src/jdpostct.c src/jdsample.c src/jdtrans.c  src/jerror.c
  src/jfdctflt.c src/jfdctfst.c src/jfdctint.c src/jidctflt.c src/jidctfst.c src/jidctint.c src/jidctred.c
  src/jquant1.c  src/jquant2.c  src/jutils.c   src/jmemmgr.c  src/jmemnobs.c)

if(WITH_ARITH_ENC OR WITH_ARITH_DEC)
  list(APPEND sources src/jaricom.c)
endif()

if(WITH_ARITH_ENC)
  list(APPEND sources src/jcarith.c)
endif()

if(WITH_ARITH_DEC)
  list(APPEND sources src/jdarith.c)
endif()

if(WITH_SIMD)
  add_definitions(-DWITH_SIMD)
  add_subdirectory(src/simd)
  list(APPEND sources src/simd/jsimd_x86_64.c)
  set_source_files_properties(${SIMD_OBJS} PROPERTIES GENERATED 1)
else()
  list(APPEND sources src/jsimd_none.c)
endif()

# Library
add_library(${PROJECT_NAME} ${headers} ${sources} ${SIMD_OBJS})

if(WITH_SIMD)
  add_dependencies(${PROJECT_NAME} simd)
endif()

# Include Directories
target_include_directories(${PROJECT_NAME}
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR} src)

# Install Target
install(TARGETS ${PROJECT_NAME} DESTINATION lib/$<LOWER_CASE:$<CONFIG>>)

# Install Headers
install(FILES ${headers} DESTINATION include)
