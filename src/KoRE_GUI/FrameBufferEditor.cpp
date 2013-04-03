#include "FrameBufferEditor.h"
#include "KoRE/ResourceManager.h"

koregui::FrameBufferEditor::FrameBufferEditor(koregui::FrameBufferItem* stage,
                                              QWidget *parent)
                                            : _currentitem(stage),
                                              QWidget(parent) {
  ui.setupUi(this);
  connect(ui.newbutton, SIGNAL(clicked()), this, SLOT(addNewFramebuffer()));
  connect(ui.framebufferselect,
          SIGNAL(currentTextChanged(const QString&)),
          this,
          SLOT(framebufferChanged(const QString&)));

  connect(ui.addTagetButton, SIGNAL(clicked()), this, SLOT(addTarget()));

  _currentbuffer = _currentitem->getFrameBuffer();
  refresh();
}

koregui::FrameBufferEditor::~FrameBufferEditor() {

}

void koregui::FrameBufferEditor::addNewFramebuffer(void) {
  uint cnt = kore::ResourceManager::getInstance()->getFramebuffers().size();
  char buf[100];
  sprintf(buf, "Framebuffer_%i", cnt);
  kore::FrameBuffer* frabuf = new kore::FrameBuffer(std::string(buf));
  kore::ResourceManager::getInstance()->addFramebuffer(frabuf);
  ui.tableWidget->verticalHeader()->hide();
  refresh();
}

void koregui::FrameBufferEditor::addTarget(void) {
  ui.tableWidget->setRowCount(ui.tableWidget->rowCount() + 1);
  QComboBox* combo = new QComboBox();
  //combo->addItem()
  ui.tableWidget->setCellWidget(ui.tableWidget->rowCount() -1, 0, combo);
}

void koregui::FrameBufferEditor::refresh(void) {
  disconnect(ui.framebufferselect,
    SIGNAL(currentTextChanged(const QString&)),
    this,
    SLOT(framebufferChanged(const QString&)));
  ui.framebufferselect->clear();
  std::vector<kore::FrameBuffer*> bufferlist
    = kore::ResourceManager::getInstance()->getFramebuffers();
  if(!_currentbuffer) {
    ui.framebufferselect->addItem("<empty>");
  }
  for (uint i = 0; i < bufferlist.size(); i++) {
    ui.framebufferselect->addItem(bufferlist[i]->getName().c_str());
  }


  QComboBox* combo = new QComboBox();
  ui.tableWidget->setCellWidget(0,0,combo);

  connect(ui.framebufferselect,
    SIGNAL(currentTextChanged(const QString&)),
    this,
    SLOT(framebufferChanged(const QString&)));
  
}

void koregui::FrameBufferEditor::framebufferChanged(const QString& name) {
  _currentbuffer = kore::ResourceManager::getInstance()
    ->getFramebuffer(ui.framebufferselect->currentText().toStdString());
  // TODO(dospelt) get actual attachment info and display it
  _currentitem->setFrameBuffer(_currentbuffer);
  refresh();
}
