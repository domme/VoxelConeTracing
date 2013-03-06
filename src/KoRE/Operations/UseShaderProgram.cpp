#include "KoRE/Operations/UseShaderProgram.h"
#include "KoRE/RenderManager.h"

kore::UseShaderProgram::UseShaderProgram()
  : _program(NULL), 
    Operation() {
}

kore::UseShaderProgram::UseShaderProgram(const ShaderProgram* program)
  : Operation() {
    connect(program);
}

kore::UseShaderProgram::~UseShaderProgram() {
}

void kore::UseShaderProgram::connect(const ShaderProgram* program) {
  _program = program;
}

void kore::UseShaderProgram::execute(void) {
  _renderManager->useShaderProgram(_program->getProgramLocation());
}

void kore::UseShaderProgram::update(void) {
}

void kore::UseShaderProgram::reset(void) {
}

bool kore::UseShaderProgram::isValid(void) {
  return _program != NULL;
}

bool kore::UseShaderProgram::dependsOn(const void* thing) {
  return _program == thing;
}
