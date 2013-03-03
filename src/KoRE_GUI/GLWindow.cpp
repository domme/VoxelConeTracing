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
/* \author Dominik Ospelt                                               */
/************************************************************************/

#include "KoRE_GUI/GLWindow.h"

#include <QGuiApplication>
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QTimer>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "KoRE/ShaderProgram.h"
#include "KoRE/Mesh.h"
#include "KoRE/ResourceManager.h"
#include "KoRE/RenderManager.h"

GLWindow::GLWindow(QScreen *screen) : QWindow(screen) {

    QSurfaceFormat cformat;
    cformat.setDepthBufferSize(24);
    cformat.setMajorVersion(4);
    cformat.setMinorVersion(2);
    cformat.setProfile(QSurfaceFormat::CoreProfile);
    
    resize(800,600);
    setSurfaceType(OpenGLSurface);
    create();

    _context = new QOpenGLContext;
    _context->setFormat(cformat);
    _context->create();

    connect(this,SIGNAL(widthChanged(int)), this, SLOT(resizeGL()));
    connect(this,SIGNAL(heightChanged(int)), this, SLOT(resizeGL()));

    initializeGL();
    resizeGL();

    /*QTimer* timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start();*/
}

GLWindow::~GLWindow() {
}

void GLWindow::initializeGL() {
    _context->makeCurrent(this);

    // initialize glew
    glewExperimental = GL_TRUE;
    if (glewInit()) {
      kore::Log::getInstance()->write("[ERROR] glew could not be initialized");
      QGuiApplication::quit();
    }

    // get GL info
    kore::Log::getInstance()->write(
        "Render Device: %s\n",
        reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    kore::Log::getInstance()->write(
        "Vendor: %s\n",
        reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    kore::Log::getInstance()->write(
        "OpenGL version: %s\n",
        reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    kore::Log::getInstance()->write(
        "GLSL version: %s\n",
        reinterpret_cast<const char*>
        (glGetString(GL_SHADING_LANGUAGE_VERSION)));

    glViewport(0, 0, width(), height());
    glClearColor(0.5,0.5,0.5,1);
}

void GLWindow::resizeGL() {
    _context->makeCurrent(this);
    kore::RenderManager::getInstance()->setRenderResolution(glm::ivec2(width(), height()));
    glViewport(0, 0, width(), height());
    paintGL();
}

void GLWindow::paintGL() {
  _context->makeCurrent(this);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //kore::RenderManager::getInstance()->renderFrame();
  _context->swapBuffers(this);
}

void GLWindow::update() {
    paintGL();
}

void GLWindow::keyPressEvent(QKeyEvent * evnt) {
    //if (evnt->key() == Qt::Key_Escape) QGuiApplication::quit();
}
