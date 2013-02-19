#include "SceneViewer.h"
#include <QtWidgets/QApplication>
//#include "KoRE/SceneManager.h"
//#include "KoRE/ResourceManager.h"
//#include "KoRE/RenderManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //kore::ResourceManager::getInstance()->loadScene("./assets/meshes/TestEnv.dae");
    //SceneViewer viewer(kore::SceneManager::getInstance()->getRootNode().get());
    kore::SceneViewer* viewer = new kore::SceneViewer();
    viewer->show();
    return app.exec();
}
