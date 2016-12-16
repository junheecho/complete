#-*- coding: utf-8 -*-
##git log --name-only --pretty=format:"COMMIT%n%aN%n%aD" --author-date-order --reverse
##input new empty line at the end
import sys
import os
import random
from datetime import datetime

inputFile = "bison"
f = open(inputFile)
commits = []

## construct log list
line = ""
name = ""
date = datetime.today()
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

f = open(name,'w')
for commit in commits:
    timeLine = "Date:"+str(int(commit[0].timestamp()))+"\n"
    f.write(timeLine)
    f.write("Author:"+commit[1]+"\n")
    for file in commit[2]:
        f.write(file+"\n")
    f.write("\n")
f.close()
