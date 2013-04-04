/*  
  Copyright @ 2012 The KoRE Project

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

#ifndef KORE_SRC_KORE_INDEXEDBUFFER_H_
#define KORE_SRC_KORE_INDEXEDBUFFER_H_

#include "KoRE/Common.h"
#include "KoRE/BaseResource.h"

namespace kore {
  class IndexedBuffer : public BaseResource {
    public:
      IndexedBuffer();
      virtual ~IndexedBuffer();

      bool create(GLuint bufferTarget,
                  GLsizeiptr sizeInBytes, GLuint usageHint,
                  GLvoid* data = NULL,
                  const std::string& name = "");

      bool uploadData(uint offset, uint sizeInBytes, GLvoid* data);

      inline const GLuint getHandle() const {return _handle;}
      inline const std::string& getName() const {return _name;}
      //inline const GLuint getDataType() const {return _dataType;}
      inline const GLuint getTarget() const {return _bufferTarget;}
      inline const GLsizeiptr getSizeInBytes() const {return _sizeInBytes;}
      inline const GLuint getUsageHint() const {return _usageHint;}

    private:
      std::string _name;
      GLuint _bufferTarget;
     // GLuint _dataType;
      GLsizeiptr _sizeInBytes;
      GLuint _usageHint;
      GLuint _handle;


      void destroy();
  };
}

#endif  // KORE_SRC_KORE_INDEXEDBUFFER_H_
