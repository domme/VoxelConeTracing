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

#include "KoRE_GUI/GLWidget.h"

#include <QGuiApplication>
#include <QKeyEvent>
#include <QTimer>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "KoRE/ShaderProgram.h"
#include "KoRE/SceneManager.h"
#include "KoRE/ResourceManager.h"
#include "KoRE/RenderManager.h"

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent) {

    QGLFormat cformat;
    cformat.setDepthBufferSize(24);
    cformat.setVersion(5,8);
    cformat.setProfile(QGLFormat::CoreProfile);

    this->setFormat(cformat);
    resize(800,600);

    QTimer* timer = new QTimer(this);
    /*timer->setTimerType(Qt::TimerType::PreciseTimer);*/
    timer->setInterval(16);
    connect(timer, SIGNAL(timeout()), this, SLOT(repaint()));
    //this->paintGL();
    timer->start();
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);
}

GLWidget::~GLWidget() {
}

void GLWidget::initializeGL() {
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
}

void GLWidget::resizeGL(int x, int y) {
    //kore::RenderManager::getInstance()->setRenderResolution(glm::ivec2(width(), height()));
    glViewport(0, 0, width(), height());
    paintGL();
}

void GLWidget::paintGL() {
  // TODO all GL handling is provided by KoRE itself
  glClearColor(0.1,0.1,0.1,1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  kore::RenderManager::getInstance()->renderFrame();
  //swapBuffers();
}

void GLWidget::keyPressEvent(QKeyEvent * evnt) {
    if (evnt->key() == Qt::Key_Escape) QGuiApplication::quit();
    QGLWidget::keyPressEvent(evnt);
}
