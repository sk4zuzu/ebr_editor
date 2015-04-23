
cmake_minimum_required (VERSION 2.8)

project (ebr_editor)

file (GLOB SOURCES src/*.c)

add_executable (ebr_editor ${SOURCES})

install (TARGETS ebr_editor DESTINATION bin)

# vim:ts=4:sw=4:et:
