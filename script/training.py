from random import random,choice,randint,uniform,shuffle,getrandbits
from math import sin,cos,tan,atan,sinh,cosh,tanh,exp,sqrt,log
from copy import deepcopy
import sys
import os
from datetime import datetime

size = 5

try:
  population = int(os.sys.argv[2])
except:
  population = 10

try:
  generation = int(os.sys.argv[3])
except:
  generation = 5

checkcount = 0

##unary = ["sqrt","log","-","abs","sin","cos","tan","atan","sinh","cosh","tanh","exp"]
unary = ["sqrt","log","-","abs","exp"]
##rpn_unary = ["sqrt","log","~","abs","sin","cos","tan","atan","sinh","cosh","tanh","exp"]
rpn_unary = ["sqrt","log","~","abs","exp"]
binary = ["**","+","-","*","/"]
rpn_binary = ["^","+","-","*","/"]
terminals = ["x1","x2","0.8","0.7"]
#constant : 0, variable 1~57
minSample = [[None,None,None] for i in range(10)]
minAvgRank = [float('inf') for i in range(10)]
calculatedTree = []
calculatedRoot = []
pool = []


####################################
##makefile list with lexicographical order
def constructFileList(commits):
    fileList = set()
    fileList.add("root")
    for commit in commits:
        for files in commit[2]:
            path = files.split("/")
            file = "root"
            fileList.add(file)
            for name in path:
                file += "/"+name
                fileList.add(file)
    ret = list(fileList)
    ret.sort()
    return ret

def constructDictionary(flist):
    dictionary = dict()
    for i in range(len(flist)):
        dictionary[flist[i]] = i
    return dictionary

def constructFileNameList(flist):
    fileNameList = list()
    fileNameList.append("root")
    for i in range(1,len(flist)):
        filename = flist[i][flist[i].rindex("/")+1:]
        fileNameList.append(filename)
    return fileNameList



def constructFileSystem(flist):
    fsystem = [[-1]]
    for i in range(1,len(flist)):
        parentfile = flist[i][:flist[i].rindex("/")]
        parPosit = flist.index(parentfile)
        fsystem[parPosit].append(i)
        fsystem.append([parPosit])
    return fsystem

def totRank(sample):
    global history, fileNameList, fileSystem
    frequency = [0]*len(fileSystem)
    modifiedtime = [0]*len(fileSystem)
    fileExist = [False]*len(fileSystem)
    fileExist[0] = True
    totRank = 0
    totTry = 0
    totOne = 0
    for k in range(len(history)-1):
        if history[k][2] > history[k+1][2]:
            print("histoError",history[k][2]," ",history[k+1][2]," ",k)
            quit()
    for k in range(len(history)):
        if k%1000 == 0 :
            print("history",k,"th history")
        log = history[k][0]
        route = []
        idx = log
        whilecount = 0
        while idx != -1:
            whilecount += 1
            if whilecount > 2000:
                exit(1)
            route.insert(0,idx)
            if not fileExist[idx]:
                fileExist[idx] = True
                modifiedtime[idx] = history[k][2]
            idx = fileSystem[idx][0]
        for i in range(0,len(route)-1):
            sidx = route[i]
            didx = route[i+1]
            firstchar = fileNameList[didx][0]
            Files = list(filter(lambda x, fe = fileExist, fn = fileNameList, char = firstchar:
                                fe[x]&(fileNameList[x][0] == char),
                                fileSystem[sidx][1:]))
            
            sortedFiles = []
            curtime = history[k][2]
            sortedFiles = frequencyRanking(Files,curtime,fileSystem,frequency,modifiedtime,sample)
            frequency[didx] += 1


            totTry += 1
            
            if len(Files) == 1:
                totOne += 1
            
            totRank += sortedFiles.index(didx)+1

    print("Total tries:", totTry, "Total count:", totRank, "# unambiguous:", totOne, "Amb count:", totRank - totOne)
    return totRank/totTry
                
##def constructRandomHistory(commits,dictionary):
##    random.seed()
##    history = []
##    for commit in commits:
##        temp = list(commit[2])
##        for i in range(len(commit[2]),0,-1):
##            name = "root/"+temp.pop(random.randrange(i))
##            history.append(dictionary[name])
##    return history

def constructHistory(commits,dictionary):
    history = []
    for commit in commits:
        for file in commit[2]:
            name = "root/"+file
            history.append((dictionary[name],commit[1],commit[0]))
    return history
            
    
def frequencyRanking(files,curtime,fsystem,frequency,modifiedtime,sample):
##    minValue = frequency[min(files, key = lambda s,f = frequency:f[s])]
##    if minValue < 0:
##        for file in files:
##            frequency[file] = -minValue+frequency[file]
    for file in files:
        x1 = frequency[file]
        x2 = (curtime - modifiedtime[file])/3600/24
        try:
            frequency[file] = eval(sample[2])
        except:
            print(x1," ",x2)
            quit()
    for file in files:
        modifiedtime[file] = curtime
    freq = list(map(lambda s,f = frequency:(-f[s],s),files))
    freq.sort()
    return list(map(lambda s:s[1],freq))



####################################

def fitness(sample) :
    global minAvgRank, minSample, checkcount
    print(" ",sample[2])
    checkcount += 1
    avgRank = 0
    try:
        avgRank = totRank(sample)
    except:
        avgRank = float('inf')
    if avgRank < minAvgRank[9]:
        i = 9
        while sample[0] != minSample[i][0]:
            i -= 1
            if i == -1 :
                break
        if i == -1:        
            minAvgRank.pop()
            minSample.pop()
            i = 8
            while avgRank < minAvgRank[i] :
                i -= 1
                if i == -1 :
                    break
            minAvgRank.insert(i+1,avgRank)
            minSample.insert(i+1,deepcopy(sample))
        
    print("calnum",checkcount, " ",avgRank)
    return avgRank

##def climb(constant,idx,unit,sample):
##  error = (mse(constant,sample),0)
##  constant_new = constant[:]
##  newerror = 0.0
##  for i in range(1,3):
##      constant_new[idx] = constant[idx]+ i*unit
##      newerror = (mse(constant_new,sample),i)
##      if newerror[0] < error[0]:
##          error = newerror
##          
##      constant_new[idx] = constant[idx]- i*unit
##      newerror = (mse(constant_new,sample),-i)
##      if newerror[0] < error[0]:
##          error = newerror
##  if error[1] == 0:
##      return 0
##  else:
##      constant[idx] += unit * error[1]
##      return 1

##def fitnessUpdate(sample):
##  constant = [uniform(-2.0,2.0) for _ in range(sample[3])]
##  for _ in range(sample[3]):
##      idx = randint(0,sample[3]-1)
##      #print(idx)
##      unit = 1.0
##      while unit > 0.01:
##          climb(constant,idx,unit,sample)
##          unit /= 5
##          #print(unit)
##  sample[1] = mse(constant,sample)

##def lastclimb(constant,idx,unit,sample):
##  error = (mse(constant,sample),0)
##  constant_new = constant[:]
##  newerror = 0.0
##  constant_new[idx] = constant[idx] + unit
##  newerror = (mse(constant_new,sample),1)
##  if newerror[0] < error[0]:
##      error = newerror
##      
##  constant_new[idx] = constant[idx] - unit
##  newerror = (mse(constant_new,sample),-1)
##  if newerror[0] < error[0]:
##      error = newerror
##  if error[1] == 0:
##      return 0
##  else:
##      constant[idx] += unit * error[1]
##      return 1

##def lastOptimise(constant,sample):
##  list = [i for i in range(sample[3])]
##  while True:
##      shuffle(list)
##      sw = 1
##      count = 0
##      print(list)
##      for i in list:
##          while lastclimb(constant,i,0.0001,sample) and count <= 100:
##              count += 1
##              sw = 0
##          print(i)
##          print(errMin)
##          print(RPN(sample[0])
##      if sw:
##          break
    

def isConstant(tree):
    if isinstance(tree,int):
        if tree == 0 or tree == 1:
            return False
        else:
            return True

    if len(tree) == 2:
        return isConstant(tree[1])
    else:
        return isConstant(tree[1]) and isConstant(tree[2])
    
def createNewTree(size):
    global terminals,unary,binary
    if size == 0:
        return randint(0,len(terminals)-1)
    terminal = randint(0,2)
    if terminal == 0:
        return [randint(0,len(unary)-1),createNewTree(size-1)]
    if terminal == 1:
        left = randint(0,size-1)
        return [randint(0,len(binary)-1),createNewTree(left),createNewTree(size-1-left)]
    if terminal == 2:
        constant = randint(0,1)
        if constant == 0:
            return [randint(0,len(binary)-1),randint(2,3),createNewTree(size-1)]
        else:
            return [randint(0,len(binary)-1),createNewTree(size-1),randint(2,3)]
    
def equationFromTree(tree):
    global calculatedTree
    calculatedTree = tree
    if isinstance(tree, int):
        return terminals[tree]
            
    if len(tree) == 2:
        if tree[0] < 2:
            return unary[tree[0]] + "(abs(" + equationFromTree(tree[1]) + "))"
        return unary[tree[0]] + "(" + equationFromTree(tree[1]) + ")"
    else:
        if tree[0] == 0:
            return "abs(" + equationFromTree(tree[1]) + ")" + binary[tree[0]] + "(" + equationFromTree(tree[2]) + ")"
        return "(" + equationFromTree(tree[1]) + ")" + binary[tree[0]] + "(" + equationFromTree(tree[2]) + ")"

    
def RPN(tree):
  if isinstance(tree, int):
      return terminals[tree]
          
  if len(tree) == 2:
      if tree[0] < 2:
          return RPN(tree[1]) + " abs " + rpn_unary[tree[0]]
      return RPN(tree[1]) + " " + rpn_unary[tree[0]]
  else:
      if tree[0] == 0:
          return RPN(tree[1]) + " abs " + RPN(tree[2]) + " " + rpn_binary[tree[0]]
      return RPN(tree[1]) + " " + RPN(tree[2]) + " " + rpn_binary[tree[0]]
        
def createOneSample(size):
    nt = createNewTree(size)
    return [nt,None,equationFromTree(nt)]

def createOneSampleFromTree(tree):
    global calculatedRoot
    calculatedRoot = tree
    return [tree,None,equationFromTree(tree)]

def constructPool(size):
    global pool
    unit = int(size/5)
    pool = [i for i in range(unit)]*4
    pool += [i for i in range(unit,unit*2)]*2
    pool += [i for i in range(unit*3,unit*4)]*1
    
def crossover(samples):
    global pool
    left = samples[choice(pool)][0]
    right = samples[choice(pool)][0]
    ldepth = randint(0,2)
    rdepth = randint(0,2)
    while (isinstance(left,int) == False) and (ldepth > 0) :
        if len(left) == 2:
            left = left[1]
        else:
            left = left[randint(1,2)]
        ldepth -= 1
    while (isinstance(right,int) == False) and (rdepth > 0) :
        if len(right) == 2:
            right = right[1]
        else:
            right = right[randint(1,2)]
        rdepth -= 1
    
    return [randint(0,len(binary)-1),deepcopy(left),deepcopy(right)]
    
def mutation(samples):
    global pool
    one = deepcopy(samples[choice(pool)][0])
    traveler = one
    pro = 10
    while True:
        if isinstance(traveler,int):
            traveler = randint(0,len(terminals)-1)
            break;
        
        if randint(0,pro) == 0:
            
            if len(traveler) == 2:
                traveler[0] = randint(0,len(unary)-1)
            else:
                traveler[0] = randint(0,len(binary)-1)
            break;
        else:
            pro -= 1
            if len(traveler) == 2:
                traveler = traveler[1]
            else:
                traveler = traveler[randint(1,2)]
    return one

inputFile = os.sys.argv[1]
f = open(inputFile,encoding='UTF8')
commits = []

## construct commit list
line = ""
name = ""
date = 0
print("construct commits")
while True:
    line = f.readline()
    if line == "\n": continue
    if not line: break
    date = int(line[5:-1])
    name = f.readline()[7:-1]
    files = []
    ##print("srcs")
    while True :
        line = f.readline()
        if line == "\n" : break
        files.append(line[0:-1])
    commits.append((date,name,files))
f.close()

print("construct file list")
fileList = constructFileList(commits)
print("construct file name list")
fileNameList = constructFileNameList(fileList)
print("construct file system")
fileSystem = constructFileSystem(fileList)

print("make a history")

dictionary = constructDictionary(fileList)
history = constructHistory(commits,dictionary)
#####################################################   
print(len(history))
minAvgRank[0] = totRank([None,None,"0"])
minSample[0] = [None,None,"0"]
print(minAvgRank)

samples = list()
samples.append(createOneSampleFromTree([3, 0, [4, [2, 1]]]))
samples.append(createOneSampleFromTree(0))
while len(samples) < population:
    newsp = createOneSample(size)
    if isConstant(newsp[0]):
        continue
    if newsp in samples:
        continue
    samples.append(newsp)
for i in range(population):
    print(i)
    samples[i][1] = fitness(samples[i])
for _ in range(generation):
    samples.sort(key=(lambda x:x[1]))
    for i in range(len(samples)):
        print(samples[i])
    constructPool(population)
    
    newsamples = list()
    while len(newsamples) < int(population*3/5):
        newsp = createOneSampleFromTree(crossover(samples))
        if isConstant(newsp[0]):
            continue
        if newsp in newsamples:
            continue
        newsamples.append(newsp)
    while len(newsamples) < population:
        newsp = createOneSampleFromTree(mutation(samples))
        if isConstant(newsp[0]):
            continue
        if newsp in newsamples:
            continue
        newsamples.append(newsp)
    samples = newsamples
    for i in range(population):
        print(i)
        samples[i][1] = fitness(samples[i])
    for i in range(len(samples)):
        print(samples[i])
print(minAvgRank[0])
print(minSample[0][2])
##i=0
##name = inputFile+"_rand"+str(i)
##while os.path.exists(name) :
##    i += 1
##    name = inputFile+"_rand"+str(i)
##
##f = open(name,'w',encoding='UTF8')
##for i in range(len(minAvgRank)):
##    f.write(str(minAvgRank[i])+" "+str(minSample[i][2])+"\n")
##f.close()

# $HOME/.completion/frecency.conf
home = os.path.expanduser("~")
outputName = os.path.join(home,".completion","frecency.conf")
f = open(outputName,'w',encoding='UTF8')
f.write(RPN(minSample[0][0])+"\n")
f.close()
#mses = [mse(constant, eqs[i]) for i in range(100)]
