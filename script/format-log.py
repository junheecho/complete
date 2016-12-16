#-*- coding: utf-8 -*-
##git log --name-only --pretty=format:"COMMIT%n%aN%n%aD" --author-date-order --reverse
##input new empty line at the end
import sys
import os
import random
from datetime import datetime

inputFile = os.sys.argv[1]
f = open(inputFile,encoding='UTF8')
commits = []

## construct log list
line = ""
name = ""
date = datetime.today()
historyLen = 0
print("construct commits")
while True:
    line = f.readline()
    if line == "COMMIT\n" :
        ##print("Find commit")
        name = f.readline()[:-1]
        date = datetime.strptime(f.readline()[:-1],"%a, %d %b %Y %H:%M:%S %z")
    else :
        files = []
        ##print("srcs")
        while True :
            if not line : break
            if line == "\n" : break
            files.append(line[0:-1])
            line = f.readline()
        if len(files) > 0:
            historyLen += len(files)
            commits.append([date,name,files])
    if not line: break
f.close()

    
random.seed()
commits.sort(key=lambda s: s[0])
for commit in commits:
    temp = list(commit[2])
    new_commit = list()
    for i in range(len(commit[2]),0,-1):
        new_commit.append(temp.pop(random.randrange(i)))
    commit[2] = new_commit
            

i=0
name = inputFile+"_rand"+str(i)
while os.path.exists(name) :
    i += 1
    name = inputFile+"_rand"+str(i)

begin = int(0.9*historyLen)
beginchecked = False
historyNum = 0
f = open(name,'w',encoding='UTF8')
for commit in commits:
    if (beginchecked == False) and (historyNum > begin):
        beginchecked = True
        f.write("BEGIN_TEST\n")
    timeLine = "Date:"+str(int(commit[0].timestamp()))+"\n"
    f.write(timeLine)
    f.write("Author:"+commit[1]+"\n")
    for file in commit[2]:
        f.write(file+"\n")
    f.write("\n")
    historyNum += len(commit[2])
f.close()
