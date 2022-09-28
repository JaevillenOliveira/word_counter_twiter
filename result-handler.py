import readline
import pandas as pd
import numpy as np

class HandleWordCounterTimeLog:
    def __init__(self):
        self.datadictReal = {}
        self.datadictUser = {}
        self.datadictSys = {}
        self.readLines()
        self.toCsv()
        
    def readLines(self):
        with open('timelog.txt') as f:
            lines = f.readlines()
            for line in lines:
                splitLine = line.split()
                if(len(splitLine) > 0):
                    if(splitLine[0] == '#Threads'):
                        curThreadsDictReal = self.datadictReal[int(splitLine[1])] = {}
                        curThreadsDictUser = self.datadictUser[int(splitLine[1])] = {}
                        curThreadsDictSys = self.datadictSys[int(splitLine[1])] = {}
                    elif (splitLine[0] == '#ProblemSize'):
                        curPrblmSizeDictReal = curThreadsDictReal[int(splitLine[1])] = []
                        curPrblmSizeDictUser = curThreadsDictUser[int(splitLine[1])] = []
                        curPrblmSizeDictSys = curThreadsDictSys[int(splitLine[1])] = []
                    elif (splitLine[0] == 'real'):
                        curPrblmSizeDictReal.append(splitLine[1])
                    elif (splitLine[0] == 'user'):
                        curPrblmSizeDictUser.append(splitLine[1])
                    elif (splitLine[0] == 'sys'):
                        curPrblmSizeDictSys.append(splitLine[1])
                        

    def toCsv(self):
        for keyth, th in self.datadictReal.items():
            for keypblm, pblm in th.items():
                self.datadictReal[keyth][keypblm] = self.getMean(pblm)
        for keyth, th in self.datadictUser.items():
            for keypblm, pblm in th.items():
                self.datadictUser[keyth][keypblm] = self.getMean(pblm)
        for keyth, th in self.datadictSys.items():
            for keypblm, pblm in th.items():
                self.datadictSys[keyth][keypblm] = self.getMean(pblm)
        dfR = pd.DataFrame(self.datadictReal)
        dfU = pd.DataFrame(self.datadictUser)
        dfS = pd.DataFrame(self.datadictSys)
        dfR.to_csv('outReal.csv')
        dfU.to_csv('outUser.csv')
        dfS.to_csv('outSys.csv')

        
    def getMean(self, arr):
        for i in range(len(arr)):
            timeStr = (arr[i])[:-1]
            cleanMin = ''
            cleanSec = ''
            afterMinutes = False
            for char in timeStr:
                if(char == 'm'):
                    afterMinutes = True
                else:
                    if(afterMinutes):
                        cleanSec += char
                    else:
                        cleanMin += char

            arr[i] = (int(cleanMin)*60 + float(cleanSec))     
                
        arr.remove(max(arr))
        arr.remove(min(arr))
        mean = str(round((sum(arr) / len(arr)),3))
        
        return mean.replace('.', ',')
        
        
        
                

HandleWordCounterTimeLog()

        