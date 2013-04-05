#include "FrameBufferEditor.h"
#include "KoRE/ResourceManager.h"
#include "KoRE/Common.h"

koregui::FrameBufferEditor::FrameBufferEditor(koregui::FrameBufferItem* stage,
                                              QWidget *parent)
                                            : _currentitem(stage),
                                              _currentbuffer(NULL),
                                              QWidget(parent) {
  ui.setupUi(this);
  
  std::vector<kore::FrameBuffer*> bufferlist
    = kore::ResourceManager::getInstance()->getFramebuffers();
  if(!_currentbuffer) {
    ui.framebufferselect->addItem("<empty>", QVariant(KORE_ID_INVALID));
  }
  
  for (uint i = 0; i < bufferlist.size(); i++) {
    ui.framebufferselect->addItem(bufferlist[i]->getName().c_str(),
      QVariant(bufferlist[i]->getID()));
  }
  ui.tableWidget->verticalHeader()->hide();

  // add  Qt connections
  connect(ui.newbutton, SIGNAL(clicked()), this, SLOT(addNewFramebuffer()));
  connect(ui.framebufferselect,
    SIGNAL(currentIndexChanged(int)),
    this,
    SLOT(framebufferChanged(int)));
}

koregui::FrameBufferEditor::~FrameBufferEditor() {

}

void koregui::FrameBufferEditor::setFramebuffer(const QString& name) {
  if(name != "<empty>") {
    
    ui.framebufferselect->setCurrentText(name);
  }
}

void koregui::FrameBufferEditor::addNewFramebuffer(void) {
  uint cnt = kore::ResourceManager::getInstance()->getFramebuffers().size();
  char buf[100];
  sprintf(buf, "Framebuffer_%i", cnt);
  kore::FrameBuffer* frabuf = new kore::FrameBuffer(std::string(buf));
  kore::ResourceManager::getInstance()->addFramebuffer(frabuf);
  ui.framebufferselect->addItem(frabuf->getName().c_str(),
                                QVariant(frabuf->getID()));
  ui.framebufferselect->setCurrentText(frabuf->getName().c_str());
  refresh();
}

void koregui::FrameBufferEditor::addTarget(void) {
  /*ui.tableWidget->setRowCount(ui.tableWidget->rowCount() + 1);
  QComboBox* combo = new QComboBox();
  //combo->addItem()
  ui.tableWidget->setCellWidget(ui.tableWidget->rowCount() -1, 0, combo);*/
}

void koregui::FrameBufferEditor::refresh(void) {
  // TODO(dospelt)
  /*ui.tableWidget->clearContents();
  QComboBox* combo = new QComboBox();
  ui.tableWidget->setCellWidget(0,0,combo);*/
}

void koregui::FrameBufferEditor::framebufferChanged(int index) {
  _currentbuffer = kore::ResourceManager::getInstance()
    ->getFramebuffer(ui.framebufferselect->itemData(
      ui.framebufferselect->currentIndex()).toULongLong());
  _currentitem->setFrameBuffer(_currentbuffer);
  refresh();
}
