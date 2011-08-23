__all__ = ["InterceptIO", "StopMirPy"]

from InterceptIO import TakeOverOutputStreams
from AutoLoad import AutoLoadScripts


#print "In PyMir init"
#print "Taking over output streams"
TakeOverOutputStreams()

#print "Autoloading scripts"
#AutoLoadScripts(globalVars = autoloadedModules)
