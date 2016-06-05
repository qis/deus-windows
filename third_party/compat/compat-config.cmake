set(COMPAT_FOUND true)
set(COMPAT_VERSION "git")
get_filename_component(COMPAT_PATH "${CMAKE_CURRENT_LIST_FILE}" PATH)

if(NOT TARGET compat)
  add_library(compat INTERFACE IMPORTED)
  set_target_properties(compat PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${COMPAT_PATH}/include")
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set_target_properties(compat PROPERTIES INTERFACE_LINK_LIBRARIES "stdc++fs")
  endif()
endif()
