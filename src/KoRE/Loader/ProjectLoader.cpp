#include "ProjectLoader.h"

#include <tinyxml/tinyxml.h>
#include <tinyxml/tinystr.h>

#include "KoRE/ResourceManager.h"
#include "KoRE/SceneManager.h"
#include "KoRE/RenderManager.h"
#include "KoRE/Log.h"

kore::ProjectLoader* kore::ProjectLoader::getInstance() {
  static ProjectLoader instance;
  return &instance;
}

void kore::ProjectLoader::loadProject( const std::string& path ) const {

}

void kore::ProjectLoader::saveProject( const std::string& path ) const {
  TiXmlDocument doc;  
  //TiXmlElement* msg;
  TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "yes");
  doc.LinkEndChild(decl);

  TiXmlElement * resources = new TiXmlElement("KoRE_Resources");
  doc.LinkEndChild(resources);

  TiXmlComment * comment = new TiXmlComment();
  comment->SetValue("TestComment" );  
  resources->LinkEndChild(comment);

  /*TiXmlComment * comment = new TiXmlComment();
  comment->SetValue(" PhysicObject information " );  
  root->LinkEndChild(comment);

  for(unsigned int i = 0; i < mDynObjects->size(); i++){
    TiXmlElement * po = new TiXmlElement( "PhysicsObject" );
    po->SetAttribute("name", mDynObjects->at(i)->getName().c_str());
    root->LinkEndChild(po);

    for(int j = 0; j < mDynObjects->at(i)->getNumShapes(); j++){

      TiXmlElement * shape = new TiXmlElement( "Shape" );

      Ogre::String s;
      ShapeType st =  mDynObjects->at(i)->getShapeType(j);
      switch(st){
      case TYPE_BOX: s = "Box"; break;
      case TYPE_SPHERE: s = "Sphere"; break;
      case TYPE_CAPSULE: s = "Capsule"; break;
      }
      shape->SetAttribute("type", s.c_str());
      po->LinkEndChild(shape);
    }
  }*/

  doc.SaveFile(path.c_str()); 
}
