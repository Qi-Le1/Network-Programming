#!/usr/bin/env python3
import os

#If demo exists, remove the directory
if os.path.isdir("demo"):
	#First remove any files in directory, might have broken * expansion on command line
	for f in os.listdir("demo"):
		os.remove(os.path.join("demo",f))
	os.rmdir("demo")

#Make directory
os.mkdir("demo")

#Make some files in demo subdir
#for i in range(0,10):
for i in range(0,10000):
	#Could pick a "sane" filename, but go for near the max (assuming 255 chars)
	with open(os.path.join("demo","r"*220 + "longfilename"+str(i)+".txt"),"w") as writefile:
		writefile.write("\n")
