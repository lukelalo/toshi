#
# Stub Server for Nebula Python extension Unit tests
# Created April, 2002, Jason L. Asbahr
# 

import npython    # Nebula Python extension
import sys

class StubServer:

    def __init__(self, portname):
        # Instruct Nebula to enter Server mode
        print '>>> StubServer.__init__: Entering Server Mode, portname:', portname
        npython.log('temp.log')
        npython.server(portname)

    def __del__(self):
        # Shutdown
        print '>>> StubServer.__del__: Shutting down'
        #npython.exit()



# When run in the interpreter, create a StubServer object

if __name__ == '__main__':
    portname = 'foo'
    if len(sys.argv) > 1:
        portname = sys.argv[1]
    server = StubServer(portname)

