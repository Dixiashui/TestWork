#!/usr/bin/python

import ConfigParser
import sys
import pexpect
import re

# Parse parameters
co = ConfigParser.ConfigParser()
co.read(sys.argv[1])
prompt = co.get("conf", "prompt")
useregex = co.get("conf", "useregex")

# Connect
p = pexpect.spawn(co.get("conf", "cmd"))

print "Connecting via "+co.get("conf", "cmd")
sys.stdout.flush()

# Login
if co.get("conf", "waitlogin") == "true":
	try:
		p.expect( co.get("conf", "loginprompt") )
	except:
		print "Error connecting to target"
		sys.exit(-1)
	
	p.sendline( co.get("conf", "username"))

try:	
	# Password
	if co.get("conf", "waitpassword") == "true":
		p.expect( co.get("conf", "passprompt") )
		p.sendline( co.get("conf", "password"))

	p.expect(prompt)
	print "Connected to target"
except:
	print "Unable to login to target"
	sys.exit(-1)
	


# Auxiliar function for execute commands
def exec_cmd(cmd):
	p.sendline(cmd)
	print "< "+cmd
	sys.stdout.flush()
	p.expect(prompt)
	# print the result
	for s in p.before.split("\n")[1:]:
		print "> "+s

	sys.stdout.flush()

# Pre-commands
print "Executing pre-commands"
for i in range(0, len(co.items("precmd"))):
	exec_cmd(co.get("precmd", "cmd"+str(i))[1:-1])
	

# THE command!
print "Executing main command"
exec_cmd(co.get("resultcmd", "cmd"))

# Save result
resp = p.before.split("\n")[1:] # Discard the first line, the echoed command


# Pos-commands
print "Executing pos-commands"
for i in range(0, len(co.items("poscmd"))):
	exec_cmd(co.get("poscmd", "cmd"+str(i))[1:-1])


# Compare result
isequal = True
failedline = -1
for i in range(0, len(co.items("expected"))):
	line = co.get("expected", "line"+str(i))[1:-1]
	if useregex == "true":
		# in regex mode, each line in expected result
		# shall match, at least, one line in result.
		matches_this_regex = False
		for r in resp:
			if re.search(line, r) != None:
				matches_this_regex = True
				break # the "r" loop	

		if not matches_this_regex:
			isequal = False
			failedline = i	
			break # the main loop
	else:
		resp[i] = resp[i].replace('\t', ' ')
		if str.strip(resp[i]) != str.strip(line):
			isequal = False
			failedline = i	
			break

print "-----"
if isequal == False:
	if useregex == "true":
		print "Expected regex not found: "+co.get("expected", "line"+str(failedline))[1:-1]
	else:
		print "Error on line #"+str(failedline)
		print "Expected: "+co.get("expected", "line"+str(failedline))[1:-1]
		print "Received: "+resp[failedline]
else:
	print "Command output matches expected."

# Return
if isequal:
	sys.exit(0)
else:
	sys.exit(-1)

