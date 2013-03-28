#include "FrameBufferEditor.h"
#include "KoRE/ResourceManager.h"

koregui::FrameBufferEditor::FrameBufferEditor(kore::FrameBufferStage* stage,
                                              QWidget *parent)
                                            : _currentstage(stage),
                                              QWidget(parent) {
  ui.setupUi(this);
  refresh();
  connect(ui.newbutton, SIGNAL(clicked()), this, SLOT(addNewFramebuffer()));
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
  ui.framebufferselect->clear();
  std::vector<kore::FrameBuffer*> bufferlist
    = kore::ResourceManager::getInstance()->getFramebuffers();
  for (uint i = 0; i < bufferlist.size(); i++) {
    ui.framebufferselect->addItem(bufferlist[i]->getName().c_str());
  }
  _currentbuffer = kore::ResourceManager::getInstance()
    ->getFramebuffer(ui.framebufferselect->currentText().toStdString());
  // TODO(dospelt) get actual attachment info and display it
}
