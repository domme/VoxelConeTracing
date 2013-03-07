/*
  Copyright (c) 2012 The KoRE Project

  This file is part of KoRE.

  KoRE is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

  KoRE is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with KoRE.  If not, see <http://www.gnu.org/licenses/>.
*/

/************************************************************************/
/* \author Dominik Ospelt                                               */
/************************************************************************/

#ifndef SCENEVIEWER_H
#define SCENEVIEWER_H

#include <QGraphicsView>
#include "KoRE/SceneNode.h"
#include "KoRE_GUI/NodeItem.h"
#include "KoRE_GUI/RenderViewer.h"

namespace koregui {
  class SceneViewer : public QGraphicsView {
      Q_OBJECT

  public:
      SceneViewer(RenderViewer* renderview, QWidget *parent = 0);
      ~SceneViewer();

      void showScene(kore::SceneNode* root);

  public slots:
    void zoomIn() {scale(1.2,1.2);}
    void zoomOut() {scale(1/1.2,1/1.2);}
    void addSelectionToRenderview();


  protected:
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent * event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent * event);
    void contextMenuEvent(QContextMenuEvent *event);


  private:
      QGraphicsScene _scene;
      void clearScene(void);
      //returns pixel width of children;
      NodeItem* createNode(kore::SceneNode* sourcenode, int x, int y);
      int estimateTreeWidth(kore::SceneNode* sourcenode);
      RenderViewer* _renderview;
  };
}
#endif // SCENEVIEWER_H
