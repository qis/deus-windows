set(FREETYPE_FOUND true)
set(FREETYPE_VERSION "2.6.3")
get_filename_component(FREETYPE_ROOT "${CMAKE_CURRENT_LIST_FILE}" PATH)

find_package(zlib     REQUIRED PATHS "${FREETYPE_ROOT}/../zlib")
find_package(png      REQUIRED PATHS "${FREETYPE_ROOT}/../png")
find_package(harfbuzz REQUIRED PATHS "${FREETYPE_ROOT}/../harfbuzz")

if(NOT TARGET freetype)
  add_library(freetype STATIC IMPORTED)
  set_target_properties(freetype PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${FREETYPE_ROOT}/include"
    IMPORTED_LOCATION_DEBUG "${FREETYPE_ROOT}/lib/debug/freetype.lib"
    IMPORTED_LOCATION_RELEASE "${FREETYPE_ROOT}/lib/release/freetype.lib"
    IMPORTED_CONFIGURATIONS "DEBUG;RELEASE"
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    INTERFACE_LINK_LIBRARIES "zlib;png;harfbuzz")
endif()
