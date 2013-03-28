cmake_minimum_required(VERSION 2.8)
project(KoRE)

# For custom cmake modules.
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/lib)

include(GetOpenGLHeaders)

if(WIN32 AND NOT EXISTS ${CMAKE_SOURCE_DIR}/ext)
  message("No ext dir. Will make one.")
  #execute_process(COMMAND ${PYTHON_VER} scripts/updateExt.py)
  execute_process(COMMAND sh scripts/updateExt.msys)
endif()

check_gl_headers()

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/lib)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++0x")
endif()

file(GLOB_RECURSE SOURCES src/KoRE/*.cpp)
file(GLOB_RECURSE HEADER src/KoRE/*.h)
add_library(KoRE ${SOURCES} ${HEADER})
include_directories(src)

set(IGNORE "-build/include")

#check code stlye
if(EXISTS "/usr/bin/python2")
    set(PYTHON_VER "python2")
else()
    set(PYTHON_VER "python")
endif()


if(WIN32)
  include_directories(${CMAKE_SOURCE_DIR}/ext/include)
  add_definitions(-DGLFW_INCLUDE_GL3)
endif()

set(CMAKE_LIBRARY_PATH  "${CMAKE_SOURCE_DIR}/lib")

#if(MSVC)
#  INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/ext/include)
#  message("Hello MSVC")
#endif()

add_custom_target(style
  COMMAND ${PYTHON_VER} scripts/cpplint.py --filter=${IGNORE} ${SOURCES}
)

ADD_SUBDIRECTORY(demos)
