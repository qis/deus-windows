cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(libressl VERSION 2.3.3 LANGUAGES C)

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

# Checks
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckIncludeFiles)

if(CMAKE_SYSTEM_NAME MATCHES "OpenBSD")
  add_definitions(-DHAVE_ATTRIBUTE__BOUNDED__)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  add_definitions(-D_DEFAULT_SOURCE)
  add_definitions(-D_BSD_SOURCE)
  add_definitions(-D_POSIX_SOURCE)
  add_definitions(-D_GNU_SOURCE)
endif()

add_definitions(-DLIBRESSL_INTERNAL)
add_definitions(-DOPENSSL_NO_HW_PADLOCK)
add_definitions(-DOPENSSL_NO_ASM)

set(CMAKE_POSITION_INDEPENDENT_CODE true)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
  add_definitions(-Wno-pointer-sign)
endif()

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_DEPRECATED_NO_WARNINGS)
  add_definitions(-D_REENTRANT -D_POSIX_THREAD_SAFE_FUNCTIONS)
  add_definitions(-DCPPFLAGS -DOPENSSL_NO_SPEED -DNO_SYSLOG -DNO_CRYPT -DNOCRYPT)

  set(MSVC_DISABLED_WARNINGS_LIST
    "C4057" # C4057: 'initializing' : 'unsigned char *' differs in
            # indirection to slightly different base types from 'char [2]'
    "C4100" # 'exarg' : unreferenced formal parameter
    "C4127" # conditional expression is constant
    "C4242" # 'function' : conversion from 'int' to 'uint8_t',
            # possible loss of data
    "C4244" # 'function' : conversion from 'int' to 'uint8_t',
            # possible loss of data
    "C4706" # assignment within conditional expression
    "C4820" # 'bytes' bytes padding added after construct 'member_name'
    "C4996" # 'read': The POSIX name for this item is deprecated. Instead,
            # use the ISO C++ conformant name: _read.
    "C4267" # integer conversion, possible loss of data
    "C4273" # inconsistent dll linkage
    "C4307" # integral constant overflow
    "C4090" # different 'const' qualifiers
  )
  string(REPLACE "C" " -wd" MSVC_DISABLED_WARNINGS_STR ${MSVC_DISABLED_WARNINGS_LIST})
  foreach(flag
      CMAKE_C_FLAGS
      CMAKE_C_FLAGS_DEBUG
      CMAKE_C_FLAGS_RELEASE
      CMAKE_C_FLAGS_MINSIZEREL
      CMAKE_C_FLAGS_RELWITHDEBINFO)
    set(${flag} "${${flag}} ${MSVC_DISABLED_WARNINGS_STR}")
  endforeach()
endif()

check_function_exists(asprintf HAVE_ASPRINTF)
if(HAVE_ASPRINTF)
  add_definitions(-DHAVE_ASPRINTF)
endif()

check_function_exists(inet_pton HAVE_INET_PTON)
if(HAVE_INET_PTON)
  add_definitions(-DHAVE_INET_PTON)
endif()

check_function_exists(reallocarray HAVE_REALLOCARRAY)
if(HAVE_REALLOCARRAY)
  add_definitions(-DHAVE_REALLOCARRAY)
endif()

check_function_exists(strcasecmp HAVE_STRCASECMP)
if(HAVE_STRCASECMP)
  add_definitions(-DHAVE_STRCASECMP)
endif()

check_function_exists(strlcat HAVE_STRLCAT)
if(HAVE_STRLCAT)
  add_definitions(-DHAVE_STRLCAT)
endif()

check_function_exists(strlcat HAVE_STRLCPY)
if(HAVE_STRLCPY)
  add_definitions(-DHAVE_STRLCPY)
endif()

check_function_exists(strndup HAVE_STRNDUP)
if(HAVE_STRNDUP)
  add_definitions(-DHAVE_STRNDUP)
endif()

if(MSVC)
  set(HAVE_STRNLEN TRUE)
  add_definitions(-DHAVE_STRNLEN)
else()
  check_function_exists(strnlen HAVE_STRNLEN)
  if(HAVE_STRNLEN)
    add_definitions(-DHAVE_STRNLEN)
  endif()
endif()

check_function_exists(strsep HAVE_STRSEP)
if(HAVE_STRSEP)
  add_definitions(-DHAVE_STRSEP)
endif()

check_function_exists(timegm HAVE_TIMEGM)
if(HAVE_TIMEGM)
  add_definitions(-DHAVE_TIMEGM)
endif()

check_function_exists(arc4random_buf HAVE_ARC4RANDOM_BUF)
if(HAVE_ARC4RANDOM_BUF)
  add_definitions(-DHAVE_ARC4RANDOM_BUF)
endif()

check_function_exists(explicit_bzero HAVE_EXPLICIT_BZERO)
if(HAVE_EXPLICIT_BZERO)
  add_definitions(-DHAVE_EXPLICIT_BZERO)
endif()

check_function_exists(getauxval HAVE_GETAUXVAL)
if(HAVE_GETAUXVAL)
  add_definitions(-DHAVE_GETAUXVAL)
endif()

check_function_exists(getentropy HAVE_GETENTROPY)
if(HAVE_GETENTROPY)
  add_definitions(-DHAVE_GETENTROPY)
endif()

check_function_exists(timingsafe_bcmp HAVE_TIMINGSAFE_BCMP)
if(HAVE_TIMINGSAFE_BCMP)
  add_definitions(-DHAVE_TIMINGSAFE_BCMP)
endif()

check_function_exists(timingsafe_memcmp HAVE_TIMINGSAFE_MEMCMP)
if(HAVE_MEMCMP)
  add_definitions(-DHAVE_MEMCMP)
endif()

check_include_files(err.h HAVE_ERR_H)
if(HAVE_ERR_H)
  add_definitions(-DHAVE_ERR_H)
endif()

set(OPENSSL_LIBS ssl crypto)
if(CMAKE_HOST_WIN32)
  set(OPENSSL_LIBS ${OPENSSL_LIBS} ws2_32)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  check_library_exists(rt clock_gettime "time.h" HAVE_CLOCK_GETTIME)
  if (HAVE_CLOCK_GETTIME)
    set(OPENSSL_LIBS ${OPENSSL_LIBS} rt)
  endif()
endif()

include_directories(../include ../include/compat src)

add_subdirectory(crypto)
add_subdirectory(ssl)
add_subdirectory(tls)
