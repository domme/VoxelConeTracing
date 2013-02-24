#ifndef SCENEVIEWER_H
#define SCENEVIEWER_H

#include <QGraphicsView>
#include "KoRE/SceneNode.h"
#include "KoRE_GUI/NodeItem.h"

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


  protected:
    void keyPressEvent(QKeyEvent* evnt);

  private:
      QGraphicsScene _scene;
      void clearScene(void);
      //returns pixel width of children;
      NodeItem* createNode(kore::SceneNodePtr sourcenode, int x, int y);
      int estimateTreeWidth(kore::SceneNodePtr sourcenode);
  };
}
#endif // SCENEVIEWER_H
