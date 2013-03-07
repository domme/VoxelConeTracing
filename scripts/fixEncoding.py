#!/usr/bin/python
#coding: UTF-8

import re
from os import popen

files = popen("find src -type f").readlines()

for file in files:
  file = file.strip()
  encodingRaw = popen("file -bi %s" % file).readlines()[0].strip()
  match = re.search('.*charset=([a-z0-9-]*).*', encodingRaw)
  encoding = match.groups()[0]

  if not ("utf-8" in encoding or "binary" in encoding):
    iconvCommand = "iconv -f %s -t utf-8 %s -o %s" % (encoding, file, file)
    print (iconvCommand)
    result = popen(iconvCommand).readlines()

    if len(result) >= 1:
      print (result[0])
