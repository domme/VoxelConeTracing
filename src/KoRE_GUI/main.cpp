#include "SceneViewer.h"
#include <GL/glew.h>
#include <QtWidgets/QApplication>
#include "GLWindow.h"
#include "KoRE/SceneManager.h"
#include "KoRE/ResourceManager.h"
#include "KoRE/RenderManager.h"

int main(int argc, char *argv[])
{

    // initialize Qt
    QApplication app(argc, argv);
    //kore::ResourceManager::getInstance()->loadScene("./assets/meshes/TestEnv.dae");
    //SceneViewer viewer(kore::SceneManager::getInstance()->getRootNode().get());
    //kore::SceneViewer* viewer = new kore::SceneViewer();
    // initialize GLEW
    GLWindow win;
    win.show();
    //view.setFocus();
    kore::SceneViewer view;
    view.show();
    return app.exec();
}
