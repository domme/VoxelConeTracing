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

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string>
#include "Log.h"

kore::Log* kore::Log::getInstance(void) {
  static kore::Log theInstance;
  return &theInstance;
}

kore::Log::Log(void) {
  kLogName_ = "KoRE_Log.txt";
  FILE* pfile = 0;
  fopen_s(&pfile, kLogName_.c_str(), "w");
  if (pfile!= 0) {
    time_t rawtime;
    time(&rawtime);
    fprintf(pfile, "KoRE Log - %s\n", ctime(&rawtime));
    fclose(pfile);
  }
}

kore::Log::~Log(void) {
  FILE* pfile = 0;
  fopen_s(&pfile, kLogName_.c_str(), "a");
  if (pfile!= 0) {
    fprintf(pfile, "%s\n", "\nEnd of Log");
    fclose(pfile);
  }
}

void kore::Log::write(const char* format, ...) {
  FILE* pfile;
  va_list args;
  fopen_s(&pfile, kLogName_.c_str(), "a");
  if (pfile!= 0) {
	  va_start(args, format);
	  vfprintf(pfile, format, args);
	  va_end(args);
    fclose(pfile);
  }
}
