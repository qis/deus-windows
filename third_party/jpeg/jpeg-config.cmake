set(JPEG_FOUND true)
set(JPEG_VERSION "1.40.90")
get_filename_component(JPEG_PATH "${CMAKE_CURRENT_LIST_FILE}" PATH)

if(NOT TARGET jpeg)
  add_library(jpeg STATIC IMPORTED)
  set_target_properties(jpeg PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${JPEG_PATH}/include"
    IMPORTED_LOCATION_DEBUG "${JPEG_PATH}/lib/debug/jpeg.lib"
    IMPORTED_LOCATION_RELEASE "${JPEG_PATH}/lib/release/jpeg.lib"
    IMPORTED_CONFIGURATIONS "DEBUG;RELEASE"
    IMPORTED_LINK_INTERFACE_LANGUAGES "C")
endif()
