#!/usr/bin/python

from xml.dom.minidom import parse, parseString
dom1 = parse("KoRE Planning Diagram.vdx")

texts = dom1.getElementsByTagName("Text")

classes = []	

for text in texts:
	textnode = text.childNodes[2]
	classname = textnode.nodeValue.strip()
	blacklist = ["+", "", "1", "Was", "Veraltet"]
	if classname not in blacklist and classname[0].isupper():
		classes.append(classname)
	
uniqueClasses =	set(classes)
print (uniqueClasses)

