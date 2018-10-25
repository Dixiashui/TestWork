#!/usr/bin/env python

import os

TW_YUM_SERVER           = "10.106.129.76"
TW_YUM_PATH             = "/home/yum/testworks/"

# delete old packages

DEL_CMD = "ssh"
DEL_ARGS = " yum@"+TW_YUM_SERVER+" \"rm -fr "+TW_YUM_PATH+"*\""

try:
	os.system(DEL_CMD + DEL_ARGS)
except OSError, e:
	print "Delete failed:", e

# upload new packages to yum server

UPLOAD_CMD = "scp"
UPLOAD_ARGS = " -pr packages/* yum@"+TW_YUM_SERVER+":"+TW_YUM_PATH

try:
	    os.system(UPLOAD_CMD + UPLOAD_ARGS)
except OSError, e:
	    print "Upload failed:", e

