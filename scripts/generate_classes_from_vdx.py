#!/usr/bin/python

licenseHeader = """/*
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

"""

from xml.dom.minidom import parse, parseString

try:
	dom1 = parse("doc/KoRE Planning Diagram.vdx")
except:
	print("You are in the wrong folder! Go to the root dir, baby.")
	exit()

texts = dom1.getElementsByTagName("Text")

classes = []	

for text in texts:
	textnode = text.childNodes[2]
	classname = textnode.nodeValue.strip()
	blacklist = ["+", "", "1", "Was", "Veraltet"]
	if classname not in blacklist and classname[0].isupper():
		classes.append(classname)
	
uniqueClasses =	set(classes)

print ("Following classes will be generated and overwritten in the source folder:")
print (uniqueClasses)

for className in uniqueClasses:
	upperClassName = className.upper()

	headerFile = """#ifndef %s_H
#define %s_H

namespace KoRE {
  class %s {
   public:
    %s();
  };
}

#endif // %s_H

""" % (upperClassName, upperClassName, className, className, upperClassName)

	cppFile = """#include "%s.h"

namespace KoRE {
  %s::%s() {
  }
}

""" % (className, className, className)

	h = open('src/%s.h' % className, 'w')
	h.write(licenseHeader + headerFile)
	h.close()

	cpp = open('src/%s.cpp' % className, 'w')
	cpp.write(licenseHeader + cppFile)
	cpp.close()
