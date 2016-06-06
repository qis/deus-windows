set(TLS_SRC
  tls.c
  tls_client.c
  tls_config.c
  tls_conninfo.c
  tls_server.c
  tls_peer.c
  tls_util.c
  tls_verify.c)

if(NOT HAVE_STRCASECMP)
  set(TLS_SRC ${TLS_SRC} strsep.c)
endif()

add_library(tls STATIC ${TLS_SRC})

install(TARGETS tls DESTINATION lib/$<LOWER_CASE:$<CONFIG>>)
