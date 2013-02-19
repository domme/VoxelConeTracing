#include "SceneViewer.h"
#include <GL/glew.h>
#include <QtWidgets/QApplication>
#include "KoRE/SceneManager.h"
#include "KoRE/ResourceManager.h"
#include "KoRE/RenderManager.h"

int main(int argc, char *argv[])
{

    // initialize Qt
    QApplication app(argc, argv);
    //kore::ResourceManager::getInstance()->loadScene("./assets/meshes/TestEnv.dae");
    //SceneViewer viewer(kore::SceneManager::getInstance()->getRootNode().get());
    kore::SceneViewer* viewer = new kore::SceneViewer();
    // initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit()) {
      kore::Log::getInstance()->write("[ERROR] could not initialize glew\n");
      exit(EXIT_FAILURE);
    }
    viewer->show();
    return app.exec();
}
