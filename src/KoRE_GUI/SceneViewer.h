#ifndef SCENEVIEWER_H
#define SCENEVIEWER_H

#include <QGraphicsView>
#include "KoRE/SceneNode.h"

namespace koregui {
  class SceneViewer : public QGraphicsView {
      Q_OBJECT

  public:
      SceneViewer(QWidget *parent = 0);
      ~SceneViewer();

      void showScene(kore::SceneNodePtr root);

  public slots:
    void zoomIn() {scale(1.2,1.2);}
    void zoomOut() {scale(1/1.2,1/1.2);}

  private:
      QGraphicsScene _scene;
      void clearScene(void);
      void createNode(kore::SceneNodePtr sourcenode);
  };
}
#endif  // SCENEVIEWER_H
