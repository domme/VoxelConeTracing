/*
Copyright 2013 by KoRE-Team
TODO add a common Header Copyright text.
*/

#define GLFW_INCLUDE_GL3
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <ctime>

// gl locations
GLuint cube_vao, cube_vert_vbo, cube_norm_vbo, simple_shader;

// shader code
std::string vp_code;
std::string fp_code;

// matrices
glm::mat4 model_mat, view_mat, projection_mat;

// time
clock_t time_start;
clock_t time_current;
clock_t time_diff;
clock_t time_last;
float time_delta = 0.0f;

void initGL(void) {
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glEnable(GL_DEPTH_TEST);
}

void initGeometry(void) {
  // UnitCube
  glGenVertexArrays(1, &cube_vao);
  glBindVertexArray(cube_vao);
  static const GLfloat g_cube_vertex_buffer_data[] = {
  // front
  -1.0f, -1.0f, -1.0f,
  1.0f, -1.0f, -1.0f,
  -1.0f, 1.0f, -1.0f,
  -1.0f, 1.0f, -1.0f,
  1.0f, -1.0f, -1.0f,
  1.0f,  1.0f, -1.0f,

  // back
  -1.0f, -1.0f, 1.0f,
  1.0f, -1.0f, 1.0f,
  -1.0f, 1.0f, 1.0f,
  -1.0f, 1.0f, 1.0f,
  1.0f, -1.0f, 1.0f,
  1.0f,  1.0f, 1.0f,

  // left
  -1.0f, -1.0f, -1.0f,
  -1.0f, 1.0f, -1.0f,
  -1.0f, -1.0f, 1.0f,
  -1.0f, -1.0f, 1.0f,
  -1.0f, 1.0f, -1.0f,
  -1.0f, 1.0f, 1.0f,

  // right
  1.0f, -1.0f, -1.0f,
  1.0f, 1.0f, -1.0f,
  1.0f, -1.0f, 1.0f,
  1.0f, -1.0f, 1.0f,
  1.0f, 1.0f, -1.0f,
  1.0f, 1.0f, 1.0f,

  // top
  -1.0f, 1.0f, -1.0f,
  1.0f, 1.0f, -1.0f,
  -1.0f, 1.0f, 1.0f,
  -1.0f, 1.0f, 1.0f,
  1.0f, 1.0f, -1.0f,
  1.0f, 1.0f, 1.0f,

  // bottom
  -1.0f, -1.0f, -1.0f,
  1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f, 1.0f,
  -1.0f, -1.0f, 1.0f,
  1.0f, -1.0f, -1.0f,
  1.0f, -1.0f, 1.0f
  };
  static const GLfloat g_cube_normal_buffer_data[] = {
  // front
  0.0f, 0.0f, -1.0f,
  0.0f, 0.0f, -1.0f,
  0.0f, 0.0f, -1.0f,
  0.0f, 0.0f, -1.0f,
  0.0f, 0.0f, -1.0f,
  0.0f, 0.0f, -1.0f,

  // back
  0.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f,

  // left
  -1.0f, 0.0f, 0.0f,
  -1.0f, 0.0f, 0.0f,
  -1.0f, 0.0f, 0.0f,
  -1.0f, 0.0f, 0.0f,
  -1.0f, 0.0f, 0.0f,
  -1.0f, 0.0f, 0.0f,

  // right
  1.0f, 0.0f, 0.0f,
  1.0f, 0.0f, 0.0f,
  1.0f, 0.0f, 0.0f,
  1.0f, 0.0f, 0.0f,
  1.0f, 0.0f, 0.0f,
  1.0f, 0.0f, 0.0f,

  // top
  0.0f, 1.0f, 0.0f,
  0.0f, 1.0f, 0.0f,
  0.0f, 1.0f, 0.0f,
  0.0f, 1.0f, 0.0f,
  0.0f, 1.0f, 0.0f,
  0.0f, 1.0f, 0.0f,

  // bottom
  0.0f, -1.0f, 0.0f,
  0.0f, -1.0f, 0.0f,
  0.0f, -1.0f, 0.0f,
  0.0f, -1.0f, 0.0f,
  0.0f, -1.0f, 0.0f,
  0.0f, -1.0f, 0.0f
  };
  glGenBuffers(1, &cube_vert_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, cube_vert_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_cube_vertex_buffer_data),
    g_cube_vertex_buffer_data, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
  glGenBuffers(1, &cube_norm_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, cube_norm_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_cube_normal_buffer_data),
    g_cube_normal_buffer_data, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);
};

void initScene(void) {
  model_mat = glm::mat4(1.0f);
  model_mat = glm::translate(model_mat, glm::vec3(0.0f, 0.0f, -4.0f));
  view_mat = glm::mat4(1.0f);
  projection_mat = glm::perspective(60.0f, 800.0f/600.0f, 0.1f, 100.0f);
}

void loadShaderFile(std::string shaderFile, std::string* program) {
  FILE *code_file;
	fopen_s(&code_file, shaderFile.c_str(), "r");

	if(code_file == NULL) {
		printf("[ERROR] Could not open shader program %s\n", shaderFile);
    return;
	}

  char f_char;
  while (fread(&f_char, sizeof(char), 1, code_file) != 0) {
	  if (f_char != '\r') program->push_back(f_char);
  }
  fclose(code_file);
};

void initShader(void) {
  // read from file
  loadShaderFile("simple.vp", &vp_code);
  loadShaderFile("simple.fp", &fp_code);

  // compile vertex shader
  const GLchar *vp = vp_code.c_str();
  GLuint vpID = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vpID, 1, &vp, 0);
  glCompileShader(vpID);

  // compile fragment shader
  const GLchar *fp = fp_code.c_str();
  GLuint fpID = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fpID, 1, &fp, 0);
  glCompileShader(fpID);

  // create and link shader program
  simple_shader = glCreateProgram();
  glAttachShader(simple_shader, vpID);
  glAttachShader(simple_shader, fpID);

  glLinkProgram(simple_shader);

  // print program info
  int infologLen = 0;
  glGetProgramiv(simple_shader, GL_INFO_LOG_LENGTH, &infologLen);
  if (infologLen > 1) {
    GLchar * infoLog = new GLchar[infologLen];
    if (infoLog == NULL) {
      printf("[ERROR] Could not allocate ShaderInfoLog buffer\n");
    }
    int charsWritten = 0;
    glGetProgramInfoLog(simple_shader, infologLen, &charsWritten, infoLog);
    std::string shaderlog = infoLog;
    free(infoLog);
    printf("[ERROR] ShaderProgram Log\n%s\n", shaderlog.c_str());
  } else {
    printf("[DEBUG] ShaderProgram compiled\n");
  }

  // activate shader
  glUseProgram(simple_shader);

  // apply constant uniform params
  GLuint loc = glGetUniformLocation(simple_shader, "projection");
  void* voidp;
  voidp = glm::value_ptr(projection_mat);
  glUniformMatrix4fv(loc, 1, GL_FALSE, static_cast<GLfloat*>(voidp));
}

void renderFrame(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(simple_shader);

  GLuint loc = glGetUniformLocation(simple_shader, "model");
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model_mat));

  glBindVertexArray(cube_vao);
  glDrawArrays(GL_TRIANGLES, 0, 36);
};

int main(void) {
  int running = GL_TRUE;

  // Initialize GLFW
  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  // Open an OpenGL window
  if (!glfwOpenWindow(800, 600, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  // disable v-sync
  glfwSwapInterval(0);

  // initialize GLEW
  if (glewInit()) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  // display hardware support and versions
  int GLFWmajor, GLFWminor, GLFWrev;
  glfwGetVersion(&GLFWmajor, &GLFWminor, &GLFWrev);
  printf("Render Device: %s\n", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
  printf("Vendor: %s\n", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
  printf("OpenGL version: %s\n", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
  printf("GLSL version: %s\n", reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
  printf("GLFW version %i.%i.%i\n", GLFWmajor, GLFWminor, GLFWrev);
  printf("GLEW version: %s\n", reinterpret_cast<const char*>(glewGetString(GLEW_VERSION)));

  initGL();
  initGeometry();
  initScene();
  initShader();

  // time start
  time_start = clock();
  time_last = time_start;

  // Main loop
  while (running) {
    // time processing
    time_current = clock();
    time_diff = time_current - time_last;
    time_last = time_current;
    time_delta = static_cast<float>(time_diff *0.001);  // timesincelastframe in milliseconds

    // OpenGL rendering goes here...
    renderFrame();
    glfwSwapBuffers();

    // animation processing
    model_mat = glm::rotate(model_mat, 47.0f * time_delta,
      glm::vec3(0.73f, 1.0f, 1.0f));

    // Check if ESC key was pressed or window was closed
    running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
  }

  // Close window and terminate GLFW
  glfwTerminate();

  // Exit program
  exit(EXIT_SUCCESS);
};
