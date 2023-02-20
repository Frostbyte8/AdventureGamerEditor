#!/bin/python
import sys, os, hashlib, time
from pathlib import Path

def attemptFix():
    
    fileName = sys.argv[1]
    
    time.sleep(5)
    
    for i in range(5):
       if(os.path.exists(fileName) == True):
           break
       else:
           time.sleep(5)
    
    if(os.path.exists(fileName) == False):
        return   
        
    with open(fileName, "r") as slnFile:
        lines = slnFile.readlines()
        
    slnFile.close()
        
    with open(fileName, "w") as outFile:
        skipCount = 0
        for line in lines:
            if skipCount == 0:
                if line.find("ALL_BUILD") == -1:
                    outFile.write(line)
                else:
                    skipCount = 4
            else:
                skipCount -= 1
    outFile.close()
    return        

FILE_LOCK_NAME = '.cmake_config_fix.lock'

try:
    with open(FILE_LOCK_NAME, 'r') as f:
        print('Script is locked (Probably already running)')
        exit()

except IOError:
    with open(FILE_LOCK_NAME, 'w') as f:
        f.write('')
        f.close()

attemptFix()

os.remove(FILE_LOCK_NAME)