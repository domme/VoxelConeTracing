file(GLOB_RECURSE SOURCES *.cpp)

add_executable(MainGLFW ${SOURCES})

target_link_libraries(MainGLFW glfw GL GLEW KoRE assimp)
