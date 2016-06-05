set(ASIO_FOUND true)
set(ASIO_VERSION "1.11.0")
get_filename_component(ASIO_PATH "${CMAKE_CURRENT_LIST_FILE}" PATH)

if(NOT TARGET asio)
  add_library(asio INTERFACE IMPORTED)
  set_target_properties(asio PROPERTIES
    INTERFACE_COMPILE_DEFINITIONS "ASIO_STANDALONE=1"
    INTERFACE_INCLUDE_DIRECTORIES "${ASIO_PATH}/include")
  if(MSVC)
    set_target_properties(asio PROPERTIES
      INTERFACE_COMPILE_DEFINITIONS "ASIO_STANDALONE=1;ASIO_MSVC=1"
      INTERFACE_LINK_LIBRARIES "comctl32.lib;ws2_32.lib")
  endif()
endif()
