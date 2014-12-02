#SENG265 Assignment 04 Fall 2014
#Author: Jason Sanche 
#python script to generate a makefile for c and C++ programs
#Usage:
#       python3 makefgen.py <program folder path>
#handles #include "file" dependencies recursively, avoiding infinite loops
#does not handle files outside of specified directory: #include <file>

import sys
import os.path
import os
import re

folderpath = sys.argv[1]
os.chdir(folderpath)
mkf = open('Makefile', 'w')

#function for finding additional dependencies
def adeps(f):
	try:
		pstream = open(f)
		dd = ""
		fvisited = [f+" "]
		for line in pstream:
			if re.match("^#include[\s\t]+\"\w+.h\"",line):
				fname = line.split('"')
				if fname[1] not in os.listdir() and fname[1] not in dd:
					print ("\n"+f+" contains #include for missing file "+fname[1]+"\n")
					dd += fname[1]+" "
				elif fname[1] not in dd:
					dd += fname[1]+" "
				elif fname[1] not in fvisited:
					fvisited.append(fname[1]+" ")
					adeps(fname[1])
		pstream.close()
		return dd
		
	except:
		print("Error occurred while accessing file",f,"\n")
		reason = sys.exc_info()
		print("Reason: ", reason[1],"\n")

#begin script 
mkf.write("CC=gcc\n\n")
mkf.write('SRCS = ')
for f in os.listdir():
    if re.match("^[\w*\s*]+.c$|^[\w*\s*]+.cc$"
                "|^[\w*\s*]+.C$|^[\w*\s*]+.cpp$",f):
        mkf.write(''+ f +' ')

mkf.write("\n\n")
mkf.write("OBJS = ")
for f in os.listdir():
    if re.match("^[\w*\s*]+.c$|^[\w*\s*]+.cc$"
                "|^[\w*\s*]+.C$|^[\w*\s*]+.cpp$",f):
        fname = f.split(".")
        mkf.write(fname[0]+".o ")
mkf.write("\n\n")

mkf.write("PROG: = prog.exe\n\n$(PROG): $(OBJS)\n\t"
		"$(CC) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $(PROG)\n")

#rules for .c files and 
for f in os.listdir():
    if re.match("^[\w*\s*]+.c$",f):
    	fname = f.split(".")
    	rule=("\n"+fname[0]+".o: "+f+" "
    	+adeps(f)+" \n\t$(CC) $(CPPFLAGS) $(CFLAGS) -c "+f+"\n")
    	mkf.write(rule)
    			
#rules for .cc .C and .cpp files
for f in os.listdir():
    if re.match("^[\w*\s*]+.cc$"
                "|^[\w*\s*]+.C$|^[\w*\s*]+.cpp$",f):
    	fname = f.split(".")
    	rule=("\n"+fname[0]+".o: "+f+" "+adeps(f)+"\n"
    		"\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c "+f+"\n")
    	mkf.write(rule)

mkf.write("\nclean:\n\trm -f $(OBJS)")
mkf.close()
