#include <sstream>

#include "KoRE/IDManager.h"
#include "KoRE/Log.h"

#include "KoRE/Mesh.h"
#include "KoRE/Texture.h"
#include "KoRE/FrameBuffer.h"
#include "KoRE/ShaderProgram.h"

kore::IDManager::IDManager(void)
 : _counter(0), 
   _internalPathName("_INTERNAL_"),
   _invalidURL("_INVALID_") {
}

kore::IDManager::~IDManager(void) {

}

kore::IDManager* kore::IDManager::getInstance(void) {
  static kore::IDManager theInstance;
  return &theInstance;
}

const std::string& kore::IDManager::getURL(uint64 id) const {
  auto it = _mapURL.find(id);
  
  if (it != _mapURL.end()) {
    return it->second;
  }

  return _invalidURL;
}

const uint64 kore::IDManager::getID(const std::string& url) const {
  for (auto it = _mapURL.begin(); it != _mapURL.end(); ++it) {
    if (it->second == url) {
      return it->first;
    }
  }

  return KORE_ID_INVALID;
}

void kore::IDManager::registerURL(uint64 id, const std::string& url) {
  if (_mapURL.find(id) != _mapURL.end()) {
    Log::getInstance()->write("[WARNING] The ID %i already has an URL"
                              " registered: %s", id, url.c_str());
    return;
  }
  _mapURL[id] = url;
}

std::string kore::IDManager::genURL(const std::string& name,
                                    const std::string& filepath /* = "" */,
                                    const uint fileIndex /* = 0 */) const {
  std::stringstream ss;

  if (filepath.size() == 0) {
    ss << _internalPathName;
  } else {
    ss << filepath;
  }

  ss << "_" << name << "_" << fileIndex;

  return ss.str();
}


