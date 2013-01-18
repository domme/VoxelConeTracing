#include "core/scenenodecomponent.h"
#include "core/SceneNode.h"

kore::SceneNodeComponent::SceneNodeComponent()
  : _id(UINT_INVALID),
  _node(NULL) {

}

kore::SceneNodeComponent::~SceneNodeComponent() {

}

void kore::SceneNodeComponent::attachTo(kore::SceneNode* node) {

}

const kore::SceneNode*
  kore::SceneNodeComponent::getNode(void) const {
    return _node;
}

uint kore::SceneNodeComponent::getID(void) const {
  return _id;
}
