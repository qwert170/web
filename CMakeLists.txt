cmake_minimum_required(VERSION 3.10.2)
project(web)

aux_source_directory(. DIRSRCS)

include_directories(./ ./base ./net ./net/http)

add_subdirectory(base)
add_subdirectory(net)

add_executable(web main.cpp ${DIRSRCS})
target_link_libraries(web base net http)
