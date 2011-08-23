import sys

def StopMirPy(funName = "OnMirPyClose"):
	modules = sys.modules.values()
	#get all attributes funName from all modules
	list = [getattr(module, funName) for module in modules if hasattr(module, funName)]
	#make sure they're callable
	list = [fun for fun in list if callable(fun)]
	#call them all
	for fun in list:
		#print "Sending stop event", funName, "to module", fun
		fun()
