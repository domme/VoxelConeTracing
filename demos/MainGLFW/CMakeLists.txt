file(GLOB_RECURSE SOURCES *.cpp)

add_executable(MainGLFW ${SOURCES})

if(MSVC)
	target_link_libraries(MainGLFW
	  ${CMAKE_SOURCE_DIR}/ext/lib/GLFW.lib
	  KoRE
	  ${CMAKE_SOURCE_DIR}/ext/lib/glew32.lib
	  ${CMAKE_SOURCE_DIR}/ext/lib/assimp32.lib
	  )
else()
  target_link_libraries(MainGLFW glfw GL GLEW KoRE assimp)
endif()
