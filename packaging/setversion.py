#!/usr/bin/env python

import os
import sys
import time


print("Enter new version (ex: 5.1, default: current year.month):")
version=sys.stdin.readline()
version = version.strip()
if(version == ""):
	version = time.strftime('%y.%m',time.localtime(time.time()))

print("Enter version type(default: 1): \n	1 <--- Official \n	2 <--- Beta \n")
type=sys.stdin.readline().strip()
comments = ""
if(type == "2"):
	type = "Beta"
	print("Enter other information if there's any:")
	comments = sys.stdin.readline().strip()
	if(comments != ""):
		comments = '.' + comments
else:
	type = ""
version += type

i,o = os.popen2("svn info ~/TestWorks/TestworkR20/",bufsize=100)
revision = o.read()
revision = revision.split()
i = 0
for str in revision:
	if(str == "Revision:"):
		revision = revision[i+1].strip()
		print("revision set to %s" % revision)
		break
	i += 1

osVersion = os.popen("cat /etc/redhat-release").read()
array = osVersion.split(" ")
osVersion = '.' + array[0] + array[2].replace('.','')
	
print("Changing specs...")

for file in os.listdir("."):
	if(file.rfind("spec") != -1):
		file = file.strip()
		CMD = "cat "+ file + ' | sed "s/^Version:.*/Version: ' + version + '/"'
		CMD += ' | sed "s/^Release:.*/Release: ' + revision + osVersion + comments + '/" > i.tmp'
		os.system(CMD)
		os.system("mv i.tmp "+file)

file = "~/TestWorks/wireshark/wireshark_override/packaging/rpm/SPECS/wireshark.spec.in"
CMD = "cat "+ file + ' | sed "s/^%define rel.*/%define rel ' + revision + '/" > i.tmp'
os.system(CMD)
os.system("mv i.tmp "+file)

if(sys.argv[1] == "make"):
	os.system("make clean;make")
