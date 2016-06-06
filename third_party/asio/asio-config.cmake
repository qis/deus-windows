set(ASIO_FOUND true)
set(ASIO_VERSION "1.11.0")
get_filename_component(ASIO_PATH "${CMAKE_CURRENT_LIST_FILE}" PATH)

if(NOT TARGET asio)
  add_library(asio INTERFACE IMPORTED)
  set_target_properties(asio PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ASIO_PATH}/include"
    INTERFACE_COMPILE_DEFINITIONS "ASIO_STANDALONE=1")
  if(MSVC)
    set_target_properties(asio PROPERTIES
      INTERFACE_COMPILE_DEFINITIONS "ASIO_STANDALONE=1;ASIO_MSVC=1"
      INTERFACE_LINK_LIBRARIES "comctl32.lib;ws2_32.lib")
  endif()
endif()

find_package(libressl PATHS "${ASIO_PATH}/../libressl")

if(LIBRESSL_FOUND AND NOT TARGET asio_ssl)
  add_library(asio_ssl INTERFACE IMPORTED)
  set_target_properties(asio_ssl PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ASIO_PATH}/include"
    INTERFACE_LINK_LIBRARIES "asio;ssl")
endif()
