#!/bin/python
import subprocess, os

if(os.path.exists("CMakelists.txt")):
  with open("CMakelists.txt", "r") as cmFile:
    makeLines = cmFile.readlines()
  for curLine in makeLines:
    if curLine.find("project(") != -1:
        projectName = curLine
        break;
else:
    print("Cannot find CMakelists.txt!")
    exit()
    
fileName = projectName[8: len(projectName) - 2]
fileName += ".sln"
fileName = "out/" + fileName
print (fileName)

print("You have 15 seconds to generate the project file if you want 'ALL_BUILD' removed. If it still fails, try running strip_sub.py manually.", fileName)
p = subprocess.Popen(['python', os.path.expanduser('strip_sub.py'), fileName], close_fds=True, creationflags=0x00000008)

exit()