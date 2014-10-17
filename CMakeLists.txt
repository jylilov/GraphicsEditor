cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)

project(GraphicsEditor C)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

add_definitions(${GTK3_CFLAGS_DIRS})

include(GResource)
add_gresource(${CMAKE_CURRENT_SOURCE_DIR}/ui grapheditor.gresource.xml
        ${CMAKE_CURRENT_BINARY_DIR} UI_RESOURCES)
	
file(GLOB_RECURSE SOURCES "src/*.c")
file(GLOB_RECURSE HEADERS "src/*.h")

add_executable(graphics_editor ${SOURCES} ${HEADERS} ${UI_RESOURCES})
target_link_libraries(graphics_editor ${GTK3_LIBRARIES})