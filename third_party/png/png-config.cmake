set(PNG_FOUND true)
set(PNG_VERSION "1.6.21")
get_filename_component(PNG_ROOT "${CMAKE_CURRENT_LIST_FILE}" PATH)

find_package(zlib REQUIRED PATHS "${PNG_ROOT}/../zlib")

if(NOT TARGET png)
  add_library(png STATIC IMPORTED)
  set_target_properties(png PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${PNG_ROOT}/include"
    IMPORTED_LOCATION_DEBUG "${PNG_ROOT}/lib/debug/png.lib"
    IMPORTED_LOCATION_RELEASE "${PNG_ROOT}/lib/release/png.lib"
    IMPORTED_CONFIGURATIONS "DEBUG;RELEASE"
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    INTERFACE_LINK_LIBRARIES "zlib")
endif()
