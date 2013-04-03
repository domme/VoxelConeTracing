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

void kore::IDManager::insert(uint64 id, const std::string& url) {
  if (getURL(id) != _invalidURL) {
    Log::getInstance()->write("[WARNING] The id %i is already inserted into"
                              "the IDmanager with url %s", id, url.c_str());
    return;
  }

  _mapURL[id] = url;
}

std::string kore::IDManager::genURL(const uint64 id,
                                    const std::string& scenePath /* = "" */,
                                    const std::string& name /* = "" */) const {
  std::stringstream ss;

  if (scenePath.size() == 0) {
    ss << _internalPathName;
  } else {
    ss << scenePath;
  }

  ss << "_" << name << "_" << id;

  return ss.str();
}
