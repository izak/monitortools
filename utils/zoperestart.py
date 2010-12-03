import sys
import os
import subprocess
from time import sleep
from select import poll, POLLIN
from StringIO import StringIO
from threading import Thread, Event

class LogReaderThread(Thread):
    def __init__(self, log, monitor, signal):
        Thread.__init__(self)
        self.log = log
        self.signal = signal
        self.monitor = monitor

    def run(self):
        try:
            line = self.log.readline()
            while line:
                sys.stderr.write('.')
                if self.monitor.isSet() and \
                    line.strip().endswith("Zope Ready to handle requests"):
                    self.signal.set()
                    return
                line = self.log.readline()
        except ValueError:
            # File was closed, just roll over and die
            pass

def run(args, stdout=None):
    if stdout is None:
        stdout=subprocess.PIPE
    app = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT)
    # Close stdin, we won't use it
    app.stdin.close()

    return app

def restart(args, timeout):
    restartcmd = args + ["restart"]
    logtailcmd = args + ["logtail"]

    # Run logtail command, do this first to seek to the end of the current
    # log before restarting.
    sys.stderr.write(" flush ")
    tailapp = run(logtailcmd)
    restartedevent = Event()
    monitorevent = Event()
    reader = LogReaderThread(tailapp.stdout, monitorevent, restartedevent)
    reader.start()

    # Restart zope and wait for it to finish
    sys.stderr.write(" restart ")
    file13 = open('/dev/null', 'w')
    zopectl = run(restartcmd, stdout=file13)
    zopectl.wait()
    sys.stderr.write(" done ")

    # Now signal the Log reader thread to start looking
    monitorevent.set()

    # Wait for the thread to complete or timeout
    restartedevent.wait(timeout)

    # And stop the logtail command
    tailapp.terminate()

    if restartedevent.isSet():
        print >>sys.stderr, " success"
        return 0

    print >>sys.stderr, " alarm"
    return 1

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print >>sys.stderr, "Usage: %s zopectlscript" % sys.argv[0]
        sys.exit(3)

    timeout = int(os.environ.get('ZOPE_RESTART_TIMEOUT', '60'))
    for arg in sys.argv[1:]:
        x = restart([arg], timeout)
        if x != 0:
            sys.exit(x)
        sys.exit(1)
