import sys
#import threading
from MirPy import ConsoleLog

class InterceptIO:
    "Takes over a stream from module sys and directs it's output to the MirPy console"
    def __init__(self, name, log_type = -1):
        target = getattr(sys, name)
        self.log_type = log_type
        self.old_target = target
        setattr(sys, name, self)
        #self.lock = threading.Lock()
        
    def write(self, data):
        #self.lock.acquire()    
        ConsoleLog(str(data), self.log_type)
        #self.lock.release()
        #thread.release()
        #self.old_target.write(data)

    def read(self, data):
        pass
            
		
def TakeOverOutputStreams():
    "Takes over sys.stdout and sys.stderr and directs their output to MirPy's console"
    out = InterceptIO('stdout', -3) #3 == LOG_OUTPUT
    err = InterceptIO('stderr', -2) #2 == LOG_ERROR
