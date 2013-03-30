file(GLOB_RECURSE SOURCES *.cpp)

add_executable(MainGLFW ${SOURCES})



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
	    link_directories(${CMAKE_LIBRARY_PATH})
	    include_directories(${CMAKE_LIBRARY_PATH})
	    
        #add_library( glew32 STATIC IMPORTED)        
        #set_target_properties( glew32 PROPERTIES IMPORTED_LOCATION ${CMAKE_LIBRARY_PATH}/libglew32.a )
        
        #add_library( assimp STATIC IMPORTED)        
        #set_target_properties( assimp PROPERTIES IMPORTED_LOCATION ${CMAKE_LIBRARY_PATH}/libassimp.a )
        
        #add_library( tinyxml STATIC IMPORTED)        
        #set_target_properties( tinyxml PROPERTIES IMPORTED_LOCATION ${CMAKE_LIBRARY_PATH}/libtinyxml.a)
	    
		set(KoRE_LIBS
		#${EXT_LIBS}/assimp.a
	    #${EXT_LIBS}/libglfw.a
	    #${EXT_LIBS}/libglew32.a
	    #${EXT_LIBS}/libglew32.a
	    #${EXT_LIBS}/assimp_debug-dll_win32/assimp.lib
	    #OpenGL32
	    #glaux
	    #glu32
	    #assimp
	    opengl32
	    glfw
	    glew32
	    tinyxml
	    KoRE 
	    #p2t
	    )
	
	
  endif(MSVC)
else()
  #Linux
  set(KoRE_LIBS glfw GL GLEW KoRE assimp)
endif(WIN32)

target_link_libraries(MainGLFW tinyxml)
target_link_libraries(MainGLFW ${KoRE_LIBS})

