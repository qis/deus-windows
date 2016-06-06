file(GLOB SSL_SRC *.c)

add_library(ssl STATIC ${SSL_SRC})

install(TARGETS ssl DESTINATION lib/$<LOWER_CASE:$<CONFIG>>)
