#include "ShaderEditor.h"
#include <QLabel>
#include <QLineEdit>
#include <QToolBox>
#include <QToolButton>
#include <QFileDialog>
#include <QSizePolicy>
#include "KoRE/ResourceManager.h"
#include "KoRE/IDManager.h"

koregui::ShaderEditor::ShaderEditor(ShaderProgramItem* pass, QWidget *parent)
                                    : _currentitem(pass),
                                      _currentprogram(NULL),
                                      QWidget(parent) {
  ui.setupUi(this);
  connect(ui.newButton, SIGNAL(clicked()), this, SLOT(addNewShaderProgram()));
  connect(ui.nameEdit, SIGNAL(textEdited(const QString &)),
          this, SLOT (nameChanged(const QString&)));
  connect(ui.addShaderButton, SIGNAL(clicked()), this, SLOT(addShader()));
  connect(ui.programSelection, SIGNAL(currentIndexChanged(const QString&)),
          this, SLOT(setShaderProgram(const QString&)));
  connect(&_loadmapper, SIGNAL(mapped(int)), this, SLOT(pathButtonPressed(int)));
  connect(&_delmapper, SIGNAL(mapped(int)), this, SLOT(removeShader(int)));
  connect(ui.applyButton, SIGNAL(clicked()), this, SLOT(applyChanges()));

  ui.shaderTable->setColumnWidth(2,30);
  ui.shaderTable->setColumnWidth(3,30);
  ui.programSelection->addItem("<empty>", QVariant(KORE_ID_INVALID));
  std::vector<kore::ShaderProgram*> progs =
    kore::ResourceManager::getInstance()->getShaderPrograms();
  for (uint i = 0; i < progs.size(); i++) {
    ui.programSelection->addItem(progs[i]->getName().c_str(), QVariant(progs[i]->getID()));
  }
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
  ui.programSelection->addItem(prog->getName().c_str(),
                               QVariant(prog->getID()));
  setShaderProgram(prog->getName().c_str());
}


void koregui::ShaderEditor::addShader(void) {
  if(!_currentprogram) return;
  int row = ui.shaderTable->rowCount();
  ui.shaderTable->setRowCount(row+1);
  QComboBox* typeselect = new QComboBox();
  typeselect->addItem("VERTEX", QVariant(GL_VERTEX_SHADER));
  typeselect->addItem("FRAGMENT", QVariant(GL_FRAGMENT_SHADER));
  typeselect->addItem("GEOMETRY", QVariant(GL_GEOMETRY_SHADER));
  typeselect->addItem("TESS_CONTROL", QVariant(GL_TESS_CONTROL_SHADER));
  typeselect->addItem("TESS_EVALUATION", QVariant(GL_TESS_EVALUATION_SHADER));
  ui.shaderTable->setCellWidget(row, 0, typeselect);

  QLineEdit* path = new QLineEdit("<empty>");
  ui.shaderTable->setCellWidget(row, 1, path);
  QPushButton* tbutton = new QPushButton("...");
  ui.shaderTable->setCellWidget(row, 2, tbutton);
  connect(tbutton, SIGNAL(clicked()), &_loadmapper, SLOT(map()));
  _loadmapper.setMapping(tbutton, row);
  QPushButton* xbutton = new QPushButton("x");
  ui.shaderTable->setCellWidget(row, 3, xbutton);
  connect(xbutton, SIGNAL(clicked()), &_delmapper, SLOT(map()));
  _delmapper.setMapping(xbutton, row);
}

void koregui::ShaderEditor::nameChanged(const QString& name) {
  if (!_currentprogram) return;
  ui.programSelection->setItemText(ui.programSelection->currentIndex(), name);
  _currentprogram->setName(name.toStdString());
  _currentitem->refresh();
  _currentitem->update();
}

void koregui::ShaderEditor::refresh(void) {
  ui.shaderTable->clearContents();
  ui.shaderTable->setRowCount(0);
  if(!_currentprogram)return;
  ui.nameEdit->setText(_currentprogram->getName().c_str());
  kore::Shader* shader(NULL);
  shader = _currentprogram->getShader(GL_VERTEX_SHADER);
  if (shader) addShaderInfo(shader);
  shader = _currentprogram->getShader(GL_FRAGMENT_SHADER);
  if (shader) addShaderInfo(shader);
  shader = _currentprogram->getShader(GL_GEOMETRY_SHADER);
  if (shader) addShaderInfo(shader);
  shader = _currentprogram->getShader(GL_TESS_CONTROL_SHADER);
  if (shader) addShaderInfo(shader);
  shader = _currentprogram->getShader(GL_TESS_EVALUATION_SHADER);
  if (shader) addShaderInfo(shader);
}

void koregui::ShaderEditor::setShaderProgram(const QString& name) {
  if (name != "<empty>") {
    int removeidx = ui.programSelection->findText("<empty>");
    if (removeidx >= 0) ui.programSelection->removeItem(removeidx);
    ui.programSelection->setCurrentText(name);
    _currentprogram = kore::ResourceManager::getInstance()
      ->getShaderProgram(ui.programSelection
      ->itemData(ui.programSelection->currentIndex()).toULongLong());
    _currentitem->setShaderProgram(_currentprogram);
    refresh();
  }
}

void koregui::ShaderEditor::addShaderInfo(kore::Shader* shader) {
  int row = ui.shaderTable->rowCount();
  ui.shaderTable->setRowCount(row+1);
  QComboBox* typeselect = new QComboBox();
  typeselect->addItem("VERTEX", QVariant(GL_VERTEX_SHADER));
  typeselect->addItem("FRAGMENT", QVariant(GL_FRAGMENT_SHADER));
  typeselect->addItem("GEOMETRY", QVariant(GL_GEOMETRY_SHADER));
  typeselect->addItem("TESS_CONTROL", QVariant(GL_TESS_CONTROL_SHADER));
  typeselect->addItem("TESS_EVALUATION", QVariant(GL_TESS_EVALUATION_SHADER));
  typeselect-> setCurrentIndex(typeselect->findData(QVariant(shader->getType())));
  ui.shaderTable->setCellWidget(row, 0, typeselect);

  QLineEdit* path = new QLineEdit("<empty>");
  path->setText(shader->getName().c_str());
  ui.shaderTable->setCellWidget(row, 1, path);
  QPushButton* tbutton = new QPushButton("...");
  ui.shaderTable->setCellWidget(row, 2, tbutton);
  connect(tbutton, SIGNAL(clicked()), &_loadmapper, SLOT(map()));
  _loadmapper.setMapping(tbutton, row);
  QPushButton* xbutton = new QPushButton("x");
  ui.shaderTable->setCellWidget(row, 3, xbutton);
  connect(xbutton, SIGNAL(clicked()), &_delmapper, SLOT(map()));
  _delmapper.setMapping(xbutton, row);
}

void koregui::ShaderEditor::applyChanges(void) {
  if(!_currentprogram) return;
  _currentprogram->removeShaders();
  QLineEdit* path;
  QComboBox* stype;
  for (int i = 0; i < ui.shaderTable->rowCount(); i++) {
     path = static_cast<QLineEdit*>(ui.shaderTable->cellWidget(i,1));
     stype = static_cast<QComboBox*>(ui.shaderTable->cellWidget(i,0));
     std::string bla = path->text().toStdString();
     GLenum typ = stype->itemData(stype->currentIndex()).toUInt();
    _currentprogram->loadShader(bla,typ);
  }
  _currentprogram->init();
  _currentitem->refresh();
  _currentitem->setShaderProgram(_currentprogram);
  _currentitem->update();
  refresh();
}

void koregui::ShaderEditor::pathButtonPressed(int row) {
   QString fileName
    = QFileDialog::getOpenFileName(this,
      tr("Load Shader"),
      "./assets/",
      tr("Shader (*.vp *.vert *.fp *.frag *.gp *.geom *.shader)"));
   QString s = QDir::currentPath();
   
   QLineEdit* ledit = static_cast<QLineEdit*>(ui.shaderTable->cellWidget(row,1));
   if (fileName != "") ledit->setText(fileName);
}

void koregui::ShaderEditor::removeShader(int row) {
  ui.shaderTable->removeRow(row);
}
