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

# Build KoRE library
file(GLOB_RECURSE SOURCES src/KoRE/*.cpp)
file(GLOB_RECURSE HEADER src/KoRE/*.h)
add_library(KoRE ${SOURCES} ${HEADER})
include_directories(src)
include_directories(D:/GitHub/KoRE/ext/include/tinyxml)
include_directories(demos/contrib)

# Build Clipper library (?!)
file(GLOB_RECURSE SOURCES ext/contrib/clipper/*.cpp)
file(GLOB_RECURSE HEADER ext/contrib/clipper/*.hpp)
add_library(clipper ${SOURCES} ${HEADER})

# Build Tinyxml library (?!)
#file(GLOB_RECURSE SOURCES ext/include/tinyxml/*.cpp)
#file(GLOB_RECURSE HEADER ext/include/tinyxml/*.h)
#add_library(tinyxml ${SOURCES} ${HEADER})
#include_directories( ext/include/tinyxml)

# Build poly2Tri library (?!)
file(GLOB_RECURSE SOURCES ext/contrib/poly2tri/poly2tri/*.cpp)
file(GLOB_RECURSE HEADER ext/contrib/poly2tri/poly2tri/*.h)
#add_library(p2t ${SOURCES} ${HEADER})
#SET_TARGET_PROPERTIES(p2t PROPERTIES LINKER_LANGUAGE C)



######### 
# Link external libraries to KoRE
set(CMAKE_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/lib)
link_directories(${CMAKE_LIBRARY_PATH})
include_directories(${CMAKE_LIBRARY_PATH})
	    
add_library( glew32 STATIC IMPORTED)        
set_target_properties( glew32 PROPERTIES IMPORTED_LOCATION ${CMAKE_LIBRARY_PATH}/libglew32.dll.a )
target_link_libraries(KoRE glew32)

add_library( assimp STATIC IMPORTED)        
set_target_properties( assimp PROPERTIES IMPORTED_LOCATION ${CMAKE_LIBRARY_PATH}/libassimp.a )
target_link_libraries(KoRE assimp) 

#add_library( tinyxml STATIC IMPORTED)        
#set_target_properties( tinyxml PROPERTIES IMPORTED_LOCATION ${CMAKE_LIBRARY_PATH}/tinyxml.lib)
#target_link_libraries(KoRE tinyxml)

target_link_libraries(KoRE opengl32) 
target_link_libraries(KoRE clipper) 
target_link_libraries(KoRE p2t) 

 
##########

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

set(CMAKE_LIBRARY_PATH  ${CMAKE_SOURCE_DIR}/lib)

#if(MSVC)
#  INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/ext/include)
#  message("Hello MSVC")
#endif()

add_custom_target(style
  COMMAND ${PYTHON_VER} scripts/cpplint.py --filter=${IGNORE} ${SOURCES}
)

ADD_SUBDIRECTORY(demos)
