project(FLTK_TEST CXX)
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

set(SDL_STATIC ON)
set(BUILD_SHARED_LIBS OFF)
set(SDLTTF_VENDORED OFF)

#find_package(OpenGL REQUIRED)
#find_package(GLEW REQUIRED)

add_subdirectory(lib/fltk)

#include_directories( src lib/fltk ${OPENGL_INCLUDE_DIRS}  ${GLEW_INCLUDE_DIRS} )
include_directories( src lib/fltk )

add_executable(fltk_text_editor src/fltk_text_editor.cpp)

if(WIN32)
    target_link_options(fltk_text_editor PRIVATE -static -lwinpthread -static-libgcc -static-libstdc++)
else()
    target_link_options(fltk_text_editor PRIVATE -static-libgcc -static-libstdc++)
endif()

target_link_libraries(fltk_text_editor PRIVATE fltk::fltk )


