file(GLOB_RECURSE SOURCES *.cpp)

add_executable(MainGLFW ${SOURCES})

#if(MSVC)
if(WIN32)
  link_directories(${CMAKE_SOURCE_DIR}/ext/lib/)
	target_link_libraries(MainGLFW
	  GLFW.lib
	  KoRE
	  glew32.lib
	  assimp32.lib
	  )
else()
  target_link_libraries(MainGLFW glfw GL GLEW KoRE assimp)
endif()
