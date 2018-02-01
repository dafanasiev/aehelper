cmake_minimum_required(VERSION 3.9)
project(aehelper C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_EXE_LINKER_FLAGS "-Wl,-export-dynamic")

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)

INCLUDE_DIRECTORIES(${GTK3_INCLUDE_DIRS})
LINK_DIRECTORIES(${GTK3_LIBRARY_DIRS})
ADD_DEFINITIONS(${GTK3_CFLAGS_OTHER})

add_executable(aehelper main.c gb_string.h)
TARGET_LINK_LIBRARIES(aehelper ${GTK3_LIBRARIES})

configure_file("ui.glade" "ui.glade" COPYONLY)

