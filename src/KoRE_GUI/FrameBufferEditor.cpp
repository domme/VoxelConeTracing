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
  refresh();
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
  connect(ui.framebufferselect,
    SIGNAL(currentTextChanged(const QString&)),
    this,
    SLOT(framebufferChanged(const QString&)));
  
}

void koregui::FrameBufferEditor::framebufferChanged( const QString & name ) {
  _currentbuffer = kore::ResourceManager::getInstance()
    ->getFramebuffer(ui.framebufferselect->currentText().toStdString());
  // TODO(dospelt) get actual attachment info and display it
  _currentitem->setFrameBuffer(_currentbuffer);
  refresh();
}
