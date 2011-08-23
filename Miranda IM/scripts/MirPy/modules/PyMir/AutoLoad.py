import sys
import os
import imp
import MirPy

def LoadCallback(globalVars, dirName, fnames):
	"""Callback for AutoLoadScripts.
	"""
	for fileName in fnames:
		try:
			(module, ext) = os.path.splitext(fileName)
			if (ext in [".pyc", ".py"]):
			#don't load it if it's already loaded
				try:
					#print 'Trying to find out if following module has been installed: ', (module, ext)
					sys.modules[module]
				except KeyError:
					#print 'The module has not been installed yet. ', KeyError
					(fin, pathName, description) = imp.find_module(module, [dirName]) #find a suitable module
					try:
						print MirPy.Translate("Importing module '%s' ...") % (module)
						importedModule = (imp.load_module(module, fin, pathName, description)) #try to load it and add it to the list of known modules
						globalVars[module] = importedModule
						
						#sys.modules[module] = importedModule
					finally:
						if fin: #if it's in a file we need to close that file no matter if it succeded or not
							fin.close()
			else:
				print MirPy.Translate("Module '%s%s' will not be loaded") % (module, ext)
		except ImportError, e: #file is not a python module
			print MirPy.Translate("Module '%s' is not a valid python module ... ignoring autoload") % (module) 
		fnames = [] #only walk in the top directory

def AutoLoadScripts(path = MirPy.AutoLoadPath, globalVars = globals()):
	"""Loads all scripts in a given path.
	If path is not given then the default autoload path is used.
	"""
	os.path.walk(path, LoadCallback, globalVars)
	MirPy.ConsoleLog(MirPy.Translate("Finished autoloading modules ..."), -4)
	MirPy.ConsoleLog("\n")