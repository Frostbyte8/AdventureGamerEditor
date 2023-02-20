#!/bin/python/
import sys, os, subprocess
from pathlib import Path

f = open("src/gitinfo.h", "w")

f.write("#ifndef __GITINFO_H__\n")
f.write("#define __GITINFO_H__\n\n")

f.write("#include <string>\n\n")

f.write("const std::string GIT_VERSION_INFO = \"")

result = subprocess.run("git rev-parse HEAD", stdout=subprocess.PIPE)
gitHash = result.stdout[0:10].decode('ascii')

f.write(str(gitHash)) 
f.write("\";\n\n")

f.write("#endif // __GITINFO_H__")
f.close()
