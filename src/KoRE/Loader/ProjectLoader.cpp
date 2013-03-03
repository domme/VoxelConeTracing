#include "ProjectLoader.h"

#include <tinyxml/tinyxml.h>
#include <tinyxml/tinystr.h>
#include <string>
#include <time.h>
#include <map>

#include "KoRE/ResourceManager.h"
#include "KoRE/SceneManager.h"
#include "KoRE/RenderManager.h"
#include "KoRE/Log.h"

kore::ProjectLoader* kore::ProjectLoader::getInstance() {
  static ProjectLoader instance;
  return &instance;
}

void kore::ProjectLoader::loadProject(const std::string& path) const {
  // TODO(dospelt)
  kore::Log::getInstance()->write("[Debug] project loading not yet implemented\n");
}

void kore::ProjectLoader::saveProject(const std::string& path) const {
  ResourceManager* ResMgr = ResourceManager::getInstance();

  TiXmlDocument doc;
  TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "yes");
  doc.LinkEndChild(decl);

  TiXmlComment * comment = new TiXmlComment();
  time_t rawtime;
  time(&rawtime);
  comment->SetValue(ctime(&rawtime));
  doc.LinkEndChild(comment);

  // Resources
  TiXmlElement* resources = new TiXmlElement("Resources");
  doc.LinkEndChild(resources);

  // Textures
  TiXmlElement* texture;
  std::map<std::string, TexturePtr>::iterator texIt;
  for(texIt = ResMgr->_textures.begin();
      texIt != ResMgr->_textures.end();
      texIt++) {
    texture = new TiXmlElement("Texture");
    STextureProperties prop = texIt->second->getProperties();
    texture->SetAttribute("name", texIt->second->getName().c_str());
    texture->SetAttribute("width", prop.width);
    texture->SetAttribute("height", prop.height);
    resources->LinkEndChild(texture);
  }

  // TODO(dospelt) the rest

  TiXmlElement* scene = new TiXmlElement("Scene");
  doc.LinkEndChild(scene);
  kore::SceneNodePtr root = kore::SceneManager::getInstance()->getRootNode();

  //saveSceneNode(scene, root);

  

  // finally, save to file
  if(doc.SaveFile(path.c_str())) {
    kore::Log::getInstance()->write("[DEBUG] writing file '%s'\n",
                                    path.c_str());
  } else {
    kore::Log::getInstance()->write("[ERROR] could not write file '%s'\n",
                                    path.c_str());
  }

  // TODO(dospelt) runtime error when deleting created pointers.
  // it seems they get automatically deleted when saving File -> tinyxmlDoku?

  // delete resources;
  // delete comment;
}
