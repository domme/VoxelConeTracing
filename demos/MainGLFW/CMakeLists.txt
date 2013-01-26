file(GLOB_RECURSE SOURCES *.cpp)

add_executable(MainGLFW ${SOURCES})

#if(MSVC)
if(WIN32)
  link_directories(${CMAKE_SOURCE_DIR}/ext/lib/)
	target_link_libraries(
      MainGLFW 
	  OpenGL32.lib 
	  ${CMAKE_SOURCE_DIR}/ext/lib/GLFW.lib 
	  ${CMAKE_SOURCE_DIR}/lib/$(Configuration)/KoRE.lib 
	  ${CMAKE_SOURCE_DIR}/ext/lib/glew32.lib 
	  ${CMAKE_SOURCE_DIR}/ext/lib/assimp32.lib 
	  )
else()
  target_link_libraries(MainGLFW glfw GL GLEW KoRE assimp)
endif()


