/*
  Copyright � 2012 The KoRE Project

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

#ifndef CORE_INCLUDE_CORE_GLERROR_H_
#define CORE_INCLUDE_CORE_GLERROR_H_

#include "KoRE/Common.h"
#include "KoRE/Log.h"

namespace kore {
  class GLerror {
  public:
    static void gl_ErrorCheckStart() {
      glGetError();
    }

    static bool gl_ErrorCheckFinish(const std::string& message) {
      GLenum err = glGetError();

      Log* pLog = Log::getInstance();
      switch( err ) {
      case GL_INVALID_ENUM: {
        pLog->write("[GL-ERROR] INVALID ENUM: %s\n", message.c_str());
      }break;
      case GL_INVALID_VALUE: {
           pLog->write("[GL-ERROR] INVALID VALUE: %s\n", message.c_str());
      }break;
      case GL_INVALID_OPERATION: {
        pLog->write("[GL-ERROR] INVALID OPERATION: %s\n", message.c_str());
      }break;
      case GL_STACK_OVERFLOW: {
        pLog->write("[GL-ERROR] STACK OVERFLOW: %s\n", message.c_str());
      }break;
      case GL_STACK_UNDERFLOW: {
        pLog->write("[GL-ERROR] STACK UNDERFLOW: %s\n", message.c_str());
      }break;
      case GL_OUT_OF_MEMORY: {
         pLog->write("[GL-ERROR] OUT OF MEMORY: %s\n", message.c_str());
      }break;
      case GL_TABLE_TOO_LARGE: {
         pLog->write("[GL-ERROR] TABLE TOO LARGE: %s\n", message.c_str());
      }break;
     }

      return err == GL_NO_ERROR;
    }

    static bool gl_ValidateFBO(const std::string& message) {
      GLenum error = glCheckFramebufferStatus(GL_FRAMEBUFFER );

      Log* pLog = Log::getInstance();

      switch( error ) {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
          pLog->write("[FRAMEBUFFER-ERROR]"
            " GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: %s\n", message.c_str());
        break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
          pLog->write("FRAMEBUFFER-ERROR]"
      " GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: %s\n", message.c_str());
        break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
          pLog->write("[FRAMEBUFFER-ERROR]"
            " GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: %s\n", message.c_str());
        break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
          pLog->write("[FRAMEBUFFER-ERROR]"
            "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: %s\n", message.c_str());
        break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
          pLog->write("[FRAMEBUFFER-ERROR]"
            "GL_FRAMEBUFFER_UNSUPPORTED: %s\n", message.c_str());
        break;
      }

      return error == GL_NO_ERROR;
    }
  };  // end class
}  // end namespace
#endif
