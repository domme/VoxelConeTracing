file(GLOB_RECURSE SOURCES *.cpp)

add_executable(VoxelConeTracing ${SOURCES})

#linking against a custom library
target_link_libraries(VoxelConeTracing D:/GitHub/KoRE/lib Assimp32d.dll)
target_link_libraries(VoxelConeTracing D:/GitHub/KoRE/lib assimp)
target_link_libraries(VoxelConeTracing D:/GitHub/KoRE/lib tinyxml.a)
target_link_libraries(VoxelConeTracing D:/GitHub/KoRE/lib glew32.a)
target_link_libraries(VoxelConeTracing D:/GitHub/KoRE/lib glfw.a)


if(WIN32)
  set(EXT_LIBS ${CMAKE_SOURCE_DIR}/ext/lib)
  set(CMAKE_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/lib)
  if(MSVC)
    link_directories(${EXT_LIBS})
	  set(KoRE_LIBS
	    KoRE
	    OpenGL32.lib
	    ${EXT_LIBS}/msvc100/GLFW.lib
	    #${CMAKE_SOURCE_DIR}/lib/$(Configuration)/KoRE.lib
	    ${EXT_LIBS}/glew32.lib
	    ${EXT_LIBS}/assimp_debug-dll_win32/assimp.lib
	  )
	else()
	#MinGW
		  set(KoRE_LIBS
	    #${EXT_LIBS}/libglfw.a
	    #${EXT_LIBS}/glew32.lib
	    #${EXT_LIBS}/libglew32.a
	    #${EXT_LIBS}/assimp_debug-dll_win32/assimp.lib
	    #OpenGL32
	    #glaux
	    #glu32
	    opengl32
	    glfw
	    glew32
	    tinyxml
	    KoRE 
	    )
	
	
  endif(MSVC)
else()
  #Linux
  set(KoRE_LIBS glfw GL GLEW KoRE assimp)
endif(WIN32)



target_link_libraries(VoxelConeTracing ${KoRE_LIBS})

