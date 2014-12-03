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

folderpath = sys.argv[1]	#command line argument specifies path to program folder
os.chdir(folderpath)		#change working directory to program folder
mkf = open('Makefile', 'w')	#create makefile to write to

#function for finding additional dependencies
def adeps(f):
	try:
		pstream = open(f) 	#open file f
		dd = ""			#list .h files found in dependency on file f
		fvisited = [f+" "]	#list of all files visited to avoid re-visiting 
		for line in pstream:
			if re.match("^#include[\s\t]+\"\w+.h\"",line): 	#find lines with: #include "file.h" 
				fname = line.split('"')			#isolate filename
				if fname[1] not in os.listdir() and fname[1] not in dd:	#if file.h not in directory
					print ("\n"+f+" contains #include for missing file "+fname[1]+"\n")
					dd += fname[1]+" "  	#print err message and add file anyway
				elif fname[1] not in dd:	#if file.h not already listed
					dd += fname[1]+" "	#add file to list
				elif fname[1] not in fvisited:	#if file.h not already visited
					fvisited.append(fname[1]+" ")	#add file to list of visited
					adeps(fname[1])		#recursively visit file.h for its dependencies etc.
		pstream.close()
		return dd	#return list of .h files which file f is directly or indirectly dependent on 
		
	except:
		print("Error occurred while accessing file",f,"\n")
		reason = sys.exc_info()
		print("Reason: ", reason[1],"\n")

#begin script 
mkf.write("CC=gcc\n\n")		
mkf.write('SRCS = ')		#define source files
for f in os.listdir():		#for files in program folder
    if re.match("^[\w*\s*]+.c$|^[\w*\s*]+.cc$"	#find c and c++ files
                "|^[\w*\s*]+.C$|^[\w*\s*]+.cpp$",f):
        mkf.write(f +' ')	#write list of source files

mkf.write("\n\n")
mkf.write("OBJS = ")		#define object files
for f in os.listdir():
    if re.match("^[\w*\s*]+.c$|^[\w*\s*]+.cc$"	#find c and c++ files
                "|^[\w*\s*]+.C$|^[\w*\s*]+.cpp$",f):
        fname = f.split(".")
        mkf.write(fname[0]+".o ")	#write file list with .o appended
mkf.write("\n\n")

mkf.write("PROG: = prog.exe\n\n$(PROG): $(OBJS)\n\t" #makefile protocols
		"$(CC) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $(PROG)\n")

#rules for .c files
for f in os.listdir():
    if re.match("^[\w*\s*]+.c$",f):	#find c source files
    	fname = f.split(".")
    	rule=("\n"+fname[0]+".o: "+f+" "#rule with additional dependencies
    	+adeps(f)+" \n\t$(CC) $(CPPFLAGS) $(CFLAGS) -c "+f+"\n")
    	mkf.write(rule)
    			
#rules for .cc .C and .cpp files
for f in os.listdir():
    if re.match("^[\w*\s*]+.cc$"	#find c++ sounrce files
                "|^[\w*\s*]+.C$|^[\w*\s*]+.cpp$",f):
    	fname = f.split(".")
    	rule=("\n"+fname[0]+".o: "+f+" "+adeps(f)+"\n"	#rule with dependencies
    		"\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c "+f+"\n")
    	mkf.write(rule)

mkf.write("\nclean:\n\trm -f $(OBJS)")
mkf.close()
