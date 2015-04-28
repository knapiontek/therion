#!/usr/bin/python

import os
import sys
import makefile

# set project location
os.chdir(os.path.dirname(__file__)+"/..") # all scripts rely on it

# actions
defined_actions = set(["-clean", "-generate", "-build", "-run", "-test"])

# modules
defined_modules = {
	"test/test" : None
}

def execute(action, module):
	print "\n>>>>>>"
	if "-clean" == action:
		makefile.clean(module)
	elif "-generate" == action:
		makefile.generate(module)
	elif "-build" == action:
		makefile.build(module)
	elif "-run" == action:
		makefile.run(module, defined_modules[module])

actions = []
modules = []
for arg in sys.argv:
	if __file__ == arg:
		pass
	elif arg in defined_actions:
		actions.append(arg)
	elif arg in defined_modules:
		modules.append(arg)
	else:
		print "\nunexpected argument: "+arg
		sys.exit()

try:
	for action in actions:
		for module in modules:
			execute(action, module)
	print "\nall done."
except:
	pass

