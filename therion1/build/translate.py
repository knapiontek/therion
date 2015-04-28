#!/usr/bin/python

import os
import re
import sys

# settings
project = os.path.dirname(__file__)+"/.."
exts = set([".hpp", ".cpp", ".h", ".c", ".py", ".pro", ".tho", ".lem"])

# patterns
case_pat = re.compile("[a-z][A-Z]")
id_pat = re.compile("[A-Za-z_][A-Za-z0-9_]+")
ext_pat = re.compile("[^\.]+(\..+)$") # split file name

def nest(on_event, target):
	for file_name in os.listdir(target):
		file_name = target+"/"+file_name
		if os.path.isdir(file_name):
			nest(on_event, file_name)
		else:
			search = ext_pat.search(file_name)
			if search and search.group(1) in exts:
				file_in = open(file_name)
				file_out = open(file_name+".ext", "w+")
				for line in file_in:
					file_out.write(on_event(line))
				file_in.close()
				file_out.close()
				if os.path.getsize(file_name) == os.path.getsize(file_name+".ext"):
					print file_name
					os.remove(file_name+".ext")
				else:
					print file_name+" [CHANGED]"
					os.rename(file_name+".ext", file_name)

def on_case(match_object):
	chars = list(match_object.group(0).lower())
	chars.insert(1, "_")
	return "".join(chars)

def on_identifier(match_object):
	identifier = match_object.group(0)
	chars = list(identifier)
	start = chars[0]
	if("A" <= start <= "Z"):
		return identifier
	else:
		return case_pat.sub(on_case, identifier)

def on_notation(line):
	return id_pat.sub(on_identifier, line)

def notation():
	print "\nnotation ..."
	nest(on_notation, ".")

def on_trail(line):
	return line.rstrip()+"\n"

def trail():
	print "\ntrailing ...\n"
	nest(on_trail, ".")

def on_replace(line):
	return line.replace(replace_src, replace_dst)

def replace(src, dst):
	print "\nreplacing ..."
	global replace_src
	global replace_dst
	replace_src = src
	replace_dst = dst
	nest(on_replace, ".")

