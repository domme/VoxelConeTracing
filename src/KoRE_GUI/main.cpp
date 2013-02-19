#include <GL/glew.h>
#include <QApplication>

#include "KoRE/SceneManager.h"
#include "KoRE/ResourceManager.h"
#include "KoRE/RenderManager.h"

#include "KoRE_GUI/SceneViewer.h"
#include "KoRE_GUI/GLWindow.h"

int main(int argc, char *argv[])
{

    // initialize Qt
    QApplication app(argc, argv);
    GLWindow win;
    win.show();
    koregui::SceneViewer view;
    view.show();

    // do stuff
    kore::ResourceManager::getInstance()->loadScene("./assets/meshes/TestEnv.dae");
    view.showScene(kore::SceneManager::getInstance()->getRootNode().get());
    return app.exec();
}
