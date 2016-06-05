set(ZIP_FOUND true)
set(ZIP_VERSION "9.1.15")
get_filename_component(ZIP_ROOT "${CMAKE_CURRENT_LIST_FILE}" PATH)

find_package(zlib REQUIRED PATHS "${ZIP_ROOT}/../zlib")

if(NOT TARGET zip)
  add_library(zip STATIC IMPORTED)
  set_target_properties(zip PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ZIP_ROOT}/include"
    IMPORTED_LOCATION_DEBUG "${ZIP_ROOT}/lib/debug/zip.lib"
    IMPORTED_LOCATION_RELEASE "${ZIP_ROOT}/lib/release/zip.lib"
    IMPORTED_CONFIGURATIONS "DEBUG;RELEASE"
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    INTERFACE_LINK_LIBRARIES "zlib")
endif()
