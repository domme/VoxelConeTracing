#include "ProjectLoader.h"

#include <stdio.h>

#include "KoRE/ResourceManager.h"
#include "KoRE/SceneManager.h"
#include "KoRE/RenderManager.h"
#include "KoRE/Log.h"

const bool kore::ProjectLoader::loadProject(const std::string& path) const {
  FILE* kore_file = fopen(path.c_str(), "rb");
  if (kore_file == NULL) {
    kore::Log::getInstance()->write(
      "[ERROR] Could not open file \"%s\"\n", path.c_str());
    return false;
  }
  // TODO(dospelt) implement in 3 steps

  // read all ressources             -> from ResourceManager
  int t;
  fread(&t ,sizeof(int), 1, kore_file);

  // read scenegraph and components  -> from SceneManager
  // read connections and operations -> from RenderManager
  fclose(kore_file);
  kore::Log::getInstance()->write(
    "[DEBUG] Reading file completed: \"%s\"\n", path.c_str());
  return true;
}

const bool kore::ProjectLoader::saveProject(const std::string& path) const {

  FILE* kore_file = fopen(path.c_str(), "wb");
  if (kore_file == NULL) {
    kore::Log::getInstance()->write(
      "[ERROR] Could not write file \"%s\"\n", path.c_str());
    return false;
  }
  // TODO(dospelt) implement in 3 steps

  // write all ressources             -> from ResourceManager
  int t = 42;
  fwrite(&t ,sizeof(int), 1, kore_file);

  // write scenegraph and components  -> from SceneManager
  // write connections and operations -> from RenderManager
  fclose(kore_file);
  kore::Log::getInstance()->write(
    "[DEBUG] Writing file completed: \"%s\"\n", path.c_str());
  return true;
}
