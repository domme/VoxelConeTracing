#include "FrameBufferEditor.h"
#include "KoRE/ResourceManager.h"

koregui::FrameBufferEditor::FrameBufferEditor(QWidget *parent)
                                             : QWidget(parent) {
  ui.setupUi(this);
  refresh();
}

koregui::FrameBufferEditor::~FrameBufferEditor() {

}

void koregui::FrameBufferEditor::addNewFramebuffer(void) {

}

void koregui::FrameBufferEditor::addNewColorTarget(void) {

}

void koregui::FrameBufferEditor::useDepthbufferTarget(bool use) {

}

void koregui::FrameBufferEditor::useStencilbufferTarget(bool use) {

}

void koregui::FrameBufferEditor::refresh(void) {
  std::vector<kore::FrameBuffer*> bufferlist
    = kore::ResourceManager::getInstance()->getFramebuffers();
  for (uint i = 0; i < bufferlist.size(); i++) {
    ui.framebufferselect->addItem(bufferlist[i]->getName().c_str());
  }
}
