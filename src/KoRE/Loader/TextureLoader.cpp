/*
  Copyright © 2012 The KoRE Project

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

#include "KoRE/Loader/TextureLoader.h"
#include "KoRE/ResourceManager.h"
#include "KoRE/Loader/lodepng.h"
#include "KoRE/Log.h"
#include "KoRE/Texture.h"

kore::TextureLoader::TextureLoader(void) {
}

kore::TextureLoader::~TextureLoader(void) {
}

kore::TextureLoader* kore::TextureLoader::getInstance() {
  static TextureLoader instance;
  return &instance;
}

kore::TexturePtr
  kore::TextureLoader::loadTexture(const std::string& filepath) {
  
  std::vector<unsigned char> imageData;
  std::vector<unsigned char> buffer;

  lodepng::load_file(buffer, filepath);

  uint width, height;
  lodepng::State pngState;
  pngState.decoder.color_convert = false;

  uint err = lodepng::decode(imageData, width, height, pngState, buffer);

  if ( err != 0) {
    kore::Log::getInstance()->write("[ERROR] Failed to load texture: "
                                    "\"%s\":\n\t%s\n",
                                    filepath,
                                    lodepng_error_text(err));
  } else {
    kore::TexturePtr tex = TexturePtr(new Texture());
    LodePNGColorMode& color = pngState.info_raw;

    tex->_xres = width;
    tex->_yres = height;
    tex->_type = GL_TEXTURE_2D;

    glGenTextures(1, &tex->_handle);
    glBindTexture(GL_TEXTURE_2D, tex->_handle);

    //Pass the actual Texture Data
    if(color.colortype == LCT_RGB)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0,
                   GL_RGB,
                   GL_UNSIGNED_BYTE,
                   reinterpret_cast<GLvoid*>(&imageData[0]));
    else if(color.colortype == LCT_RGBA)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0,
                   GL_RGBA,
                   GL_UNSIGNED_BYTE,
                   reinterpret_cast<GLvoid*>(&imageData[0]));
    ResourceManager::getInstance()->addTexture(filepath, tex);
    return tex;
  }
  return TexturePtr();
}
