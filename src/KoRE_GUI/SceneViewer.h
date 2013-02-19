#ifndef SCENEVIEWER_H
#define SCENEVIEWER_H

#include <QGraphicsView>
//#include "KoRE/SceneNode.h"

namespace kore {
  class SceneViewer : public QGraphicsView {
      Q_OBJECT

  public:
      SceneViewer(QWidget *parent = 0);
      //SceneViewer(kore::SceneNode *root, QWidget *parent = 0);
      ~SceneViewer();

      //void showTree(kore::SceneNode *root);

  public slots:
    void zoomIn() {scale(1.2,1.2);}
    void zoomOut() {scale(1/1.2,1/1.2);}

  private:
      QGraphicsScene _scene;
      //void clearTree(void);
      //void createNode(kore::SceneNode *sourcenode);
  };
}
#endif  // SCENEVIEWER_H
