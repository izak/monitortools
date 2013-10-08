#!/usr/bin/python

import re
import BaseHTTPServer
from optparse import OptionParser

class PassiveNotificationHandler(BaseHTTPServer.BaseHTTPRequestHandler):

    def run(self):
        try:
            self.parse_passive_update(self.path)
            self.send_response(200)
            self.send_header('Content-Type', 'text/plain')
            self.end_headers()
            self.wfile.write('OK')
        except ValueError:
            self.send_response(404)
            self.send_header('Content-Type', 'text/plain')
            self.end_headers()
            self.wfile.write('FAIL')

    do_GET = run

    def parse_passive_update(self, path):
        servicedata = path.split("/")[1:]
        if len(servicedata) != 4:
            raise ValueError("Path should have exactly four components")
        timestamp = int(servicedata[0])

        servername = servicedata[1]
        servicename = servicedata[2]
        message = servicedata[3]
        
        for v in (servername, servicename, message):
            if re.compile('^[A-Za-z0-9-]+$').match(v) is None:
                raise ValueError("Value must be alphanumeric")

        fp = open(self.server.nagioscmdpath, "a")
        fp.write("[%d] PROCESS_SERVICE_CHECK_RESULT;%s;%s;0;%s\n" % (timestamp, servername, servicename, message))
        fp.close()

class PassiveNotificationHTTPServer(BaseHTTPServer.HTTPServer):
    def __init__(self, server_address, RequestHandlerClass, nagioscmdpath):
        BaseHTTPServer.HTTPServer.__init__(self, server_address, RequestHandlerClass)
        self.nagioscmdpath = nagioscmdpath

def main():
    parser = OptionParser()
    parser.add_option("-b", "--bind", help="Host name or ip to bind to", default='')
    parser.add_option("-p", "--port", help="Port to listen on", default=11511)
    parser.add_option("-c", "--commandpipe", help="Location of nagios command pipe",
        default="/var/lib/nagios2/rw/nagios.cmd")
    (options, args) = parser.parse_args()


    httpd = PassiveNotificationHTTPServer((options.bind, int(options.port)),
        PassiveNotificationHandler, options.commandpipe)
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print 'Keyboard Interrupt: exiting'

if __name__ == '__main__':
    main()
