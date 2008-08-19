#!/usr/bin/python

import BaseHTTPServer

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
            if not v.isalnum():
                raise ValueError("Value must be alphanumeric")

        fp = open("/var/lib/nagios2/rw/nagios.cmd", "a")
        fp.write("[%d] PROCESS_SERVICE_CHECK_RESULT;%s;%s;0;%s\n" % (timestamp, servername, servicename, message))
        fp.close()

def main():
    httpd = BaseHTTPServer.HTTPServer(('', 11511), PassiveNotificationHandler)
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print 'Keyboard Interrupt: exiting'

if __name__ == '__main__':
    main()
