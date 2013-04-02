#include "ShaderEditor.h"
#include "KoRE/ResourceManager.h"

koregui::ShaderEditor::ShaderEditor(ShaderProgramItem* pass, QWidget *parent)
                                    : _currentitem(pass), QWidget(parent) {
  ui.setupUi(this);
  connect(ui.newButton, SIGNAL(clicked()), this, SLOT(addNewShaderProgram()));
}

koregui::ShaderEditor::~ShaderEditor() {

}

void koregui::ShaderEditor::addNewShaderProgram(void) {
  uint cnt = kore::ResourceManager::getInstance()->getShaderPrograms().size();
  char buf[100];
  sprintf(buf, "ShaderProgram_%i", cnt);
  kore::ShaderProgram* prog = new kore::ShaderProgram();
  prog->setName(std::string(buf));
  kore::ResourceManager::getInstance()->addShaderProgram(prog);
  refresh();
}

void koregui::ShaderEditor::shaderProgramChanged(const QString & name) {

}

void koregui::ShaderEditor::refresh(void) {
  //ui.shaderTable
  //_currentprogram->
}
