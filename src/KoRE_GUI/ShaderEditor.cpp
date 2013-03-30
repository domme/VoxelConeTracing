#include "ShaderEditor.h"

koregui::ShaderEditor::ShaderEditor(ShaderProgramItem* pass, QWidget *parent)
                                    : _currentitem(pass), QWidget(parent) {
  ui.setupUi(this);
}

koregui::ShaderEditor::~ShaderEditor() {

}

void koregui::ShaderEditor::addNewShaderProgram(void) {

}

void koregui::ShaderEditor::shaderProgramChanged(const QString & name) {

}

void koregui::ShaderEditor::refresh(void) {

}
