#!/usr/bin/python

import os
import sys
import translate
import makefile

# set project location
os.chdir(os.path.dirname(__file__)+"/..") # all scripts rely on it

# actions
defined_actions0 = set(["-trail", "-replace"])
defined_actions1 = set(["-clean", "-generate", "-build", "-run", "-test"])

# modules
defined_modules = {
	"langen/langen" : "-v -i src/test/sample/match.pro -o gen/langen/match.out.hpp",
	"obgen/obgen" : "-i src/test/sample/sample.tho -o gen/obgen/sample.out.hpp",
	"laygen/laygen" : "-v -i src/test/sample/login.lay -o gen/laygen/login.out.html",
	"spike/spike" : "-i init.file",
	"make/make" : "src/test/test.cpp gen/make/test.out.mak",
	"test/test" : None,
}

def execute0(action0):
	print "\n>>>>>>"
	if "-trail" == action0:
		translate.trail()
	elif "-replace" == action0:
		translate.replace("counter", "cnt")

def execute1(action1, module):
	print "\n>>>>>>"
	if "-clean" == action1:
		makefile.clean(module)
	elif "-generate" == action1:
		makefile.generate(module)
	elif "-build" == action1:
		makefile.build(module)
	elif "-run" == action1:
		makefile.run(module, defined_modules[module])

actions = []
modules = []
for arg in sys.argv:
	if __file__ == arg:
		pass
	elif arg in defined_actions0:
		execute0(arg)
	elif arg in defined_actions1:
		actions.append(arg)
	elif arg in defined_modules:
		modules.append(arg)
	else:
		print "\nunexpected argument: "+arg
		sys.exit()

try:
	for action in actions:
		for module in modules:
			execute1(action, module)
	print "\nall done."
except:
	pass

