aux_source_directory(. DIRB)
add_library(net ${DIRB})
add_subdirectory(http)
TARGET_LINK_LIBRARIES(net ${CMAKE_THREAD_LIBS_INIT})