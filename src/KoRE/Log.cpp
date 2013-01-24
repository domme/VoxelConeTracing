/*
  Copyright Â© 2012 The KoRE Project

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

#if defined WIN32 || defined WIN64
#include <Windows.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string>

#include "KoRE/Log.h"

kore::Log* kore::Log::getInstance(void) {
  static kore::Log theInstance;
  return &theInstance;
}

kore::Log::Log(void) {
  _logname = "KoRE_Log.txt";
  FILE* pfile = fopen(_logname.c_str(), "w");
  if (pfile!= 0) {
    time_t rawtime;
    time(&rawtime);
    fprintf(pfile, "KoRE Log - %s\n", ctime(&rawtime));
    fclose(pfile);
  }
}

kore::Log::~Log(void) {
  FILE* pfile = fopen(_logname.c_str(), "a");
  if (pfile!= 0) {
    fprintf(pfile, "%s\n", "\nEnd of Log");
    fclose(pfile);
  }
}

void kore::Log::write(const char* format, ...) {
  va_list args;
  FILE* pfile = fopen(_logname.c_str(), "a");
  if (pfile!= 0) {
      va_start(args, format);
      vfprintf(pfile, format, args);
      va_end(args);
      fclose(pfile);
  }

#if defined WIN32 || defined WIN64
  // Also print to VisualStudio-console
  char szBuf[ 1000 ];
  va_start(args, format);
  vsprintf(szBuf, format, args);
  va_end(args);

  OutputDebugStringA(szBuf);
#endif
}
