#!/usr/bin/python
# created on April 2009
# author: peng chao

import ConfigParser
import sys
import pexpect
import re

# Parse parameters
co = ConfigParser.ConfigParser()
co.read(sys.argv[1])
prompt = co.get("conf", "prompt")

# Connect
p = pexpect.spawn(co.get("conf", "cmd"))

print "Connecting via "+co.get("conf", "cmd")
sys.stdout.flush()

# Login
if co.get("conf", "waitlogin") == "true":
	try:
		#loginprompt =co.get("conf", "loginprompt") 
		#print  loginprompt
		p.expect( co.get("conf", "loginprompt") )
		#print p.before
	except:
		print "Error connecting to target"
		sys.exit(-1)
	
	#print "send username"
	username = co.get("conf","username") + '\r'
	#p.sendline( co.get("conf", "username"))
	p.sendline( username)

try:	
	# Password
	if co.get("conf", "waitpassword") == "true":
		#passprompt = co.get("conf","passprompt")
		#print passprompt
		p.expect( co.get("conf", "passprompt") )
		password = co.get("conf","password") + '\r'	
		#p.sendline( co.get("conf", "password"))
		p.sendline( password)
	print "waiting prompt"
	p.expect(prompt)
	print p.before
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
	#if ZZ, just return error
	if cmd == "ZZ;\r":
		sys.exit(-1)
	#print "execute not ZZ"
	p.expect(prompt)
	# print the result
	for s in p.before.split("\n")[0:]:
		print "> "+s
	sys.stdout.flush()

# Pre-commands
#print "Executing pre-commands"
#for i in range(0, len(co.items("precmd"))):
#	exec_cmd(co.get("precmd", "cmd"+str(i))[1:-1])
	

# THE command!
print "Executing main command"
command =co.get("resultcmd", "cmd") + '\r' 
exec_cmd(command)

# Save result
resp = p.before.split("\n")[1:] # Discard the first line, the echoed command

#print resp
# Pos-commands
#print "Executing pos-commands"
#for i in range(0, len(co.items("poscmd"))):
#	exec_cmd(co.get("poscmd", "cmd"+str(i))[1:-1])

# Compare result
isequal = False
expected = co.get("resultcmd","expected")
for r in resp:
	if re.search(expected,r) != None:
			isequal = True
			break # the "r" loop
	
# Return
if isequal:
	#print "match"
	sys.exit(0)
else:
	#print "don't match"
	sys.exit(-1)

