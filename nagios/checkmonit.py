#!/usr/bin/python
#
# $Id$
#
# Plugin for nagios to check that all services monitored by monit is ok
#

import sys, httplib, getopt
from elementtree.ElementTree import ElementTree, fromstring

def usage():
        sys.stderr.write("Usage: %s [-h|--help][-H|--host=hostname][-P|--port=port]\n")

def main():
    host = None
    port = None

    try:
        opts, args = getopt.getopt(sys.argv[1:], "hH:P:", ["help", "host=", "port="])
    except getopt.GetoptError:
        usage()
        sys.exit(3)

    for key, val in opts:
        if key in ("-H", "--host"):
            host = val
        if key in ("-P", "--port"):
            port = int(val)
        if key in ("-h", "--help"):
            usage()
            sys.exit(3)

    if host == None or port == None:
        usage()
        sys.exit(3)

    conn = httplib.HTTPConnection(host+":"+str(port))
    try:
        conn.request("GET", "/_status?format=xml")
    except:
        print "Unable to connect to %s port %s"%(host,port)
        sys.exit(2)

    try:
        doc = fromstring(conn.getresponse().read())
        iter = doc.findall("service")
    except:
        print "Unable to parse XML"
        sys.exit(2)
    
    mylist = []
    for element in iter:
        try:
            name = element.find("name")
            status = element.find("status")
        except:
            print "Unable to parse XML"
            sys.exit(2)

        if "0" != status.text:
            mylist.append(name.text)

    if len(mylist) > 0:
        print "Monit services down:", str(mylist)
        sys.exit(2)

    print "Monit: All services OK"
    sys.exit(0)

if __name__ == '__main__':
    main()

# vim: set ts=4 sw=4 expandtab:
