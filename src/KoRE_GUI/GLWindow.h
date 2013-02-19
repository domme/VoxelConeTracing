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
/* \author Andreas Weinmann                                             */
/************************************************************************/

#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <GL/glew.h>
#include <QWindow>

class GLWindow : public QWindow
{
    Q_OBJECT

public:
    GLWindow(QScreen *screen = 0);
    ~GLWindow();

protected:
    void keyPressEvent(QKeyEvent* evnt);

private:
    void initializeGL();

protected slots:
    void resizeGL();
    void paintGL();
    void update();

private:
    QOpenGLContext* _context;
};
#endif  // GLWINDOW_H
