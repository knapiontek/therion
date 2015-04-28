#!/usr/bin/python

import os
import re
import sys

# debug
compiler = "g++ -Wall -ggdb -O0 -c -Isrc -Igen"
linker = "g++ -Wall -ggdb -O0 -rdynamic"

# release
#compiler = "g++ -Wall -ggdb -O3 -c -Isrc -Igen"
#linker = "g++ -Wall -ggdb -O3 -rdynamic"

angle_settings = { # third-party libraries
	"bfd.h" : ["", " -lbfd"],
	"pthread.h" : ["", " -lpthread"],
	"db.h" : ["", " -ldb"],
	"cairo" : [" -I/usr/include/cairo", " -lcairo"],
}

# patterns
quote_pat = re.compile("#include\s*\"(.+\..+)\"") # quote braces ""
angle_pat = re.compile("#include\s*<(.+?)>") # angle braces <>
angle_dir_pat = re.compile("#include\s*<(.+?)/.+>") # angle braces <>
path_pat = re.compile("((.+)/.+\.)(.+)") # split file name

# find dependancies of the file (see patterns)
def find_deps(deps, file_name):
	if file_name not in deps:
		raw_quotes = [] # as found in #include pattern
		quotes, angles = [], [] # filenames related to patterns
		deps[file_name] = (quotes, angles)
		file = open("src/"+file_name)
		for line in file:
			search_quote = quote_pat.search(line)
			search_angle = angle_pat.search(line)
			search_dir_angle = angle_pat.search(line)
			if search_dir_angle and search_dir_angle.group(1) in angle_settings:
				angles.append(search_dir_angle.group(1))
			elif search_angle and search_angle.group(1) in angle_settings:
				angles.append(search_angle.group(1))
			elif search_quote:
				raw_quotes.append(search_quote.group(1))
		file.close()
		for q in raw_quotes: # go deep in deps
			if(q in deps):
				quotes.extend(deps[q][0])
			else:
				if os.path.isfile("src/"+q):
					find_deps(deps, q)
					quotes.append("src/"+q)
					quotes.extend(deps[q][0])
				if q.endswith(".h"):
					c = q.replace(".h", ".c")
					if os.path.isfile("src/"+c):
						find_deps(deps, c) # c -> h
				elif q.endswith(".hpp"):
					cpp = q.replace(".hpp", ".cpp")
					if os.path.isfile("src/"+cpp):
						find_deps(deps, cpp) # cpp -> hpp

def create_file(filename):
	dirname = os.path.dirname(filename)
	if not os.path.exists(dirname): # create nested subdir
		os.makedirs(dirname)
	return open(filename, "w+")

# generate makefile
def print_deps(deps, module): # module - executable name
	objects = ""
	all_angles = set() # all angles of all files
	file = create_file("gen/"+module+".mak")
	file.write("\nall : bin/"+module)
	file.write("\n\t@printf \"done.\\n\"")
	for key, value in deps.iteritems():
		print key
		quotes = set(value[0]) # quotes
		angles = set(value[1]) # angles
		all_angles |= angles # gather all angles
		search = path_pat.search(key)
		full_name, path, ext = search.group(1), search.group(2), search.group(3) # full_name = path + file_name
		if "c" == ext or "cpp" == ext:
			objects += " bin/"+full_name+"o"
			file.write("\nbin/"+full_name+"o : src/"+key)
			for i in quotes: file.write(" "+i)
			file.write("\n\t@printf \""+key+"\\n\"")
			file.write("\n\t@mkdir -p bin/"+os.path.dirname(full_name))
			file.write("\n\t@"+compiler)
			for i in angles: file.write(angle_settings[i][0])
			file.write(" src/"+key+" -o bin/"+full_name+"o")
	file.write("\nbin/"+module+" :"+objects)
	file.write("\n\t@mkdir -p bin/"+os.path.dirname(module))
	file.write("\n\t@"+linker)
	for a in all_angles: file.write(angle_settings[a][1])
	file.write(objects+" -o bin/"+module)
	file.write("\nclean:")
	file.write("\n\t@rm -f bin/"+module+objects)
	file.write("\n")
	file.close()

def system(command):
	if os.system(command):
		raise Exception(command)

def clean(module):
	print "\ncleaning ("+module+") ..."
	system("rm -f `find . -name *~`")
	system("rm -f `find . -name *.pyc`")
	system("make -f gen/"+module+".mak clean")

def generate(module):
	print "\ngenerating ("+module+") ...\n"
	if os.path.isfile("src/"+module+".c"):
		deps = {}
		find_deps(deps, module+".c")
		print_deps(deps, module)
	elif os.path.isfile("src/"+module+".cpp"):
		deps = {}
		find_deps(deps, module+".cpp")
		print_deps(deps, module)
	else:
		print "\nmodule "+module+".c[pp] doesn\"t exist"

def build(module):
	print "\nbuilding ("+module+") ...\n"
	system("make -f gen/"+module+".mak")

def run(module, options):
	if options is None:
		print "\nexecuting ("+module+") ..."
		system("./bin/"+module)
	else:
		print "\nexecuting ("+module+" "+options+") ..."
		system("./bin/"+module+" "+options)

