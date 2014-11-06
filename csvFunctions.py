#!/opt/bin/python3
# Author: Jason Sanche 
#Written from scratch for UVIC SENG265 Fall 2014 Assignment03
#Processes CSV file and performs manipulations on the data
#Python re-write of C code version in Assignment02
#Commands Include:
# load filename.csv, findrow B abc (row containing "abc" in col B), 
# printrow 3, evalsum C (sum column C), evalavg C (average column C), 
# save, print, stats, merge otherfile.csv, sort A (column A),
# sortnumeric B (column B), deleterow 4, quit
   
import csv
import sys
import re
import string
from operator import itemgetter

class SS:
    def __init__(self, filename):
        self.fileName = filename
        self.numRows = 0
        self.numCols = 0
        self.contents = []
        
    def readcsv(self):
        try:
            csvfile = open(self.fileName,newline='')
            csvreader = csv.reader(csvfile,delimiter=',')
            for row in csvreader:
                self.contents.append(row)
                self.numRows+=1
                self.numCols = len(row)
            csvfile.close()
        except:
            print("Error occurred while accessing file",self.fileName,"\n")
            reason = sys.exc_info()
            print("Reason: ", reason[1],"\n")

    def celledit(self,cell):
        txt = cell
        if '"' in txt or ',' in txt or ' ' in txt:
            txt = txt.replace('"','""')
            txt = '"'+txt+'"'
        else:
            txt = cell
        return txt

    def rowedit(self,row):
        ll = ''
        for i in range(self.numCols):
            ll += self.celledit(row[i])
            if i < self.numCols-1:
                ll += ", "
        return ll
    
    def printrow(self,first,last):
        if int(first) >= int(self.numRows):#out of range
            print("Out of range\n")
        elif last != None:
            if int(last) < int(self.numRows):#range end in list
                for i in range(int(first),int(last)+1):
                    print(self.rowedit(self.contents[i]))
                print()
            elif int(last) >= int(self.numRows):#range end beyond list
                for i in range(int(first),int(self.numRows)):                                                         
                    print(self.rowedit(self.contents[i]))
                print()
        else:
            print(self.rowedit(self.contents[int(first)]))
            print()

    def stats(self):
        print("File: ",self.fileName,"\nRows: ",self.numRows,
              "\nColumns: ",self.numCols,"\n")

    def evalsum(self,col):
        total = 0.0
        colnum = 0
        if re.match('[a-zA-Z]{2,}',col):#colID is more than one letter
            print("Specified column out of alphabetic range"
                  "==> Use numeric\n")
            return
        else:
            if re.match('[a-zA-Z]',col):#colID is [a-zA-Z]
                colnum = ord(str.lower(col)) - ord('a')
            else:
                colnum = int(col)
            if colnum >= self.numCols:
                print("Error: desired column is out of range\n")
                return
            elif re.match('[^0-9]+',self.contents[0][colnum]):
                print("Error: desired column has non-numeric cells\n")
                return
            for i in range(int(self.numRows)):
                if re.match('^\d*$',self.contents[i][colnum]):
                    total += float(self.contents[i][colnum])
                else:
                    print('Some cells are not valid'
                          ' for numeric evaluation')
                    return
        return total

    def evalavg(self,col):
        evalsum = self.evalsum(col)
        if evalsum == None:
            return
        else:
            return evalsum/int(self.numRows)
        
    def findrow(self,col,target):
        colnum = 0
        if re.match('[a-zA-Z]{2,}',col):#col is more than one letter
            print("Specified column out of range\n")
            return
        else:
            if re.match('[a-zA-A]',col):#col is [a-zA-Z]
                colnum = ord(str.lower(col)) - ord('a')
            else:
                colnum = int(col)
            if colnum >= self.numCols:
                print("Error: desired column is out of range\n")
                return
            for i in range(int(self.numRows)):
                if self.contents[i][colnum] == target:
                    print(target,"is in row",i,"\n")
                    return
            print("Error: There is no",target,"in column",col,"\n")

    def savecsv(self,filename):
        csvfile = open(filename,'w',newline='')
        csvwriter = csv.writer(csvfile, delimiter=',')
        for row in self.contents:
            csvwriter.writerow(row)
        csvfile.close()
    
    def sortcsv(self,col):
        colnum = 0
        if re.match('[a-zA-Z]{2,}',col):#col is more than one letter
            print("Specified column out of range\n")
            return
        else:
            if re.match('[a-zA-A]',col):#col is [a-zA-Z]
                colnum = ord(str.lower(col)) - ord('a')
            else:
                colnum = int(col)
            if colnum >= self.numCols:
                print("Error: desired column is out of range\n")
                return
            self.contents.sort(key=itemgetter(colnum))

    def deleterow(self,row):
        if 0 <= int(row) < self.numRows:
            del self.contents[int(row)]
            self.numRows -= 1
        else:
            print('Error: specified row out of range\n')

    def mergecsv(self,file):
        ss2=SS(file)
        ss2.readcsv()
        for i in range(ss2.numRows):
            self.contents.append(ss2.contents[i])
        self.numRows += ss2.numRows
                
       
class main:
    ss = None
    cmd = input("Enter a subcommand ==>")
    #requires loaded file before allowing other commands
    if re.match('^load\s\w+\.csv$|^load\s\"\w+\.csv\"$',cmd):
        if re.match('^load\s\w+\.csv$',cmd):#load [filename]
            ss = SS(cmd[5:])
            ss.readcsv()
        elif re.match('^load\s\"\w+\.csv\"$',cmd):#load ["filename"]
            ss = SS(cmd[6:-1])
            ss.readcsv()
    #no file loaded: prompt for load command until satisfied or quit
    else:
        print("Please load file\n")
        while ss == None:
            cmd = input("Enter a subcommand ==>")
            if re.match('^load\s\w+\.csv$',cmd):#load [filename]
                ss = SS(cmd[5:])
                ss.readcsv()
            elif re.match('^load\s\"\w+\.csv\"$',cmd):#load ["filename"]
                ss = SS(cmd[6:-1])
                ss.readcsv()
            elif cmd == '' or cmd == 'exit' or cmd == 'quit':
                break
    #file loaded: continuous prompt for next command
    while ss != None:         
        cmd = input("Enter a subcommand ==>")
        if cmd == '' or cmd == 'exit' or cmd == 'quit':
            break
        if re.match('^load\s\w+\.csv$',cmd):#load [filename]
            ss = SS(cmd[5:])
            ss.readcsv()
        elif re.match('^load\s\"\w+\.csv\"$',cmd):#load ["filename"]
            ss = SS(cmd[6:-1])
            ss.readcsv()
        elif re.match('^printrow\s\d+(\s\d+)?$',cmd):#printrow [range]
            cmdlist = cmd.split(' ')
            if len(cmdlist)>2:
                ss.printrow(cmdlist[1],cmdlist[2])
            else:
                ss.printrow(cmdlist[1],None)
        elif re.match('^printrow\s"\d+(\s\d+)?"$',cmd):#printrow ["range"]
            cmdlist = cmd.split('"')
            cmdlist2 = cmdlist[1].split(' ')
            if len(cmdlist2)>1:
                ss.printrow(cmdlist2[0],cmdlist2[1])
            else:
                ss.printrow(cmdlist2[0],None)
        elif cmd == 'stats':
            ss.stats()
        elif re.match('^evalsum\s[a-zA-Z0-9]+$',cmd):#evalsum [col]
            cmdlist = cmd.split(' ')
            print('Sum = ',ss.evalsum(cmdlist[1]),'\n')
        elif re.match('^evalsum\s"[a-zA-Z0-9]+"$',cmd):#evalsum ["col"]
            cmdlist = cmd.split('"')
            print('Sum = ',ss.evalsum(cmdlist[1]),'\n')
        elif re.match('^evalavg\s[a-zA-Z0-9]+$',cmd):#evalavg [col]
            cmdlist = cmd.split(' ')
            print('Average = ',ss.evalavg(cmdlist[1]),'\n')
        elif re.match('^evalavg\s"[a-zA-Z0-9]+"$',cmd):#evalavg ["col"]
            cmdlist = cmd.split('"')
            print('Average = ',ss.evalavg(cmdlist[1]),'\n')
        elif re.match('^findrow\s[a-zA-Z0-9]+\s(\w[",]*\s*)+$',cmd):#findrow [col target]
            cmdlist = cmd.split(' ',2)
            ss.findrow(cmdlist[1],cmdlist[2])
        elif re.match('^findrow\s"[a-zA-Z0-9]+\s(\w[",]*\s*)+"$',cmd):#findrow ["col target"]
            cmdlist = cmd.split('"')
            cmdlist2 = cmdlist[1].split(' ',1)
            ss.findrow(cmdlist2[0],cmdlist2[1])
        elif re.match('^save\s\w+\.\w+$',cmd):#save [newfile.csv]
            cmdlist = cmd.split(' ')
            ss.savecsv(cmdlist[1])
        elif re.match('^save\s"\w+\.\w+"$',cmd):#save ["newfile.csv"]
            cmdlist = cmd.split('"')
            ss.savecsv(cmdlist[1])
        elif re.match('^sort\s[a-zA-Z0-9]+$'
                      '|^sortnumeric\s[a-zA-Z0-9]+$',cmd):#sort/sortnumeric [col]
            cmdlist = cmd.split(' ')
            ss.sortcsv(cmdlist[1])
        elif re.match('^sort\s"[a-zA-Z0-9]+"$'
                      '|^sortnumeric\s"[a-zA-Z0-9]+"$',cmd):#sort/sortnumeric ["col"]
            cmdlist = cmd.split('"')
            ss.sortcsv(cmdlist[1])
        elif re.match('^deleterow\s[0-9]+$',cmd):#deleterow [row]
            cmdlist = cmd.split(' ')
            ss.deleterow(cmdlist[1])
        elif re.match('^deleterow\s"[0-9]+"$',cmd):#deleterow ["row"]
            cmdlist = cmd.split('"')
            ss.deleterow(cmdlist[1])
        elif re.match('^merge\s\w+\.csv$',cmd):#merge [file.csv]
            cmdlist = cmd.split(' ')
            ss.mergecsv(cmdlist[1])
        elif re.match('^merge\s"\w+\.csv"$',cmd):#merge ["file.csv"]
            cmdlist = cmd.split('"')
            ss.mergecsv(cmdlist[1])
        else:
            print("Please try again\n")#not a valid command
                
        
