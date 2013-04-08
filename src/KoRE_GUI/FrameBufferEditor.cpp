#include "FrameBufferEditor.h"
#include "KoRE/ResourceManager.h"
#include "KoRE/Common.h"
#include "KoRE/Texture.h"

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
  connect(ui.newattach, SIGNAL(clicked()), this, SLOT(addNewAttachment()));
  connect(ui.applyButton, SIGNAL(clicked()), this, SLOT(applySettings()));
  connect(ui.nameEdit, SIGNAL(textEdited(const QString&)),
          this, SLOT(nameChanged(const QString&)));
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

void koregui::FrameBufferEditor::addNewAttachment(void) {
  if(!_currentbuffer) return;
  ui.tableWidget->setRowCount(ui.tableWidget->rowCount() + 1);
  QComboBox* combo = new QComboBox();
  combo->addItem("GL_COLOR_ATTACHMENT0", QVariant(GL_COLOR_ATTACHMENT0));
  combo->addItem("GL_COLOR_ATTACHMENT1", QVariant(GL_COLOR_ATTACHMENT1));
  combo->addItem("GL_COLOR_ATTACHMENT2", QVariant(GL_COLOR_ATTACHMENT2));
  combo->addItem("GL_COLOR_ATTACHMENT3", QVariant(GL_COLOR_ATTACHMENT3));
  combo->addItem("GL_COLOR_ATTACHMENT4", QVariant(GL_COLOR_ATTACHMENT4));
  combo->addItem("GL_COLOR_ATTACHMENT5", QVariant(GL_COLOR_ATTACHMENT5));
  combo->addItem("GL_COLOR_ATTACHMENT6", QVariant(GL_COLOR_ATTACHMENT6));
  combo->addItem("GL_COLOR_ATTACHMENT7", QVariant(GL_COLOR_ATTACHMENT7));
  ui.tableWidget->setCellWidget(ui.tableWidget->rowCount() -1, 0, combo);
}

void koregui::FrameBufferEditor::applySettings(void) {
  if(!_currentbuffer) return;
  for(int i = 0; i < ui.tableWidget->rowCount(); i++) {
    kore::STextureProperties props;
    props.pixelType = ui.tableWidget->item(i,0)->data(Qt::UserRole).toUInt();
    props.format = ui.tableWidget->item(i,1)->data(Qt::UserRole).toUInt();
  }
}

void koregui::FrameBufferEditor::refresh(void) {
  ui.nameEdit->setText(_currentbuffer->getName().c_str());
  ui.tableWidget->clearContents();
}

void koregui::FrameBufferEditor::framebufferChanged(int index) {
  _currentbuffer = kore::ResourceManager::getInstance()
    ->getFramebuffer(ui.framebufferselect->itemData(
      ui.framebufferselect->currentIndex()).toULongLong());
  _currentitem->setFrameBuffer(_currentbuffer);
  int removeidx = ui.framebufferselect->findText("<empty>");
  if(removeidx >= 0) ui.framebufferselect->removeItem(removeidx);
  refresh();
}

void koregui::FrameBufferEditor::nameChanged(const QString& name) {
  if(!_currentbuffer) return;
  ui.framebufferselect->setItemText(ui.framebufferselect->currentIndex(), name);
  _currentbuffer->setName(name.toStdString());
  refresh();
}
