#!/usr/bin/python
#
# $Id$
#
# Read message from stdin and send via mira sms gateway

import sys, httplib, urllib

username = "com_obj_upfront"
password = "96393"

if len(sys.argv)!=2:
    sys.stderr.write("Usage: %s msisdn\n"%sys.argv[0])
    sys.exit(1)

# Read message body from stdin
text = sys.stdin.read()

params = urllib.urlencode({'username':username, 'password':password, 'to':sys.argv[1], 'text':text[0:160]})
conn = httplib.HTTPConnection("gateway.miranetworks.net:8080")
conn.request("GET", "/synapp-bulksms/bulksms?%s"%params)
response = conn.getresponse()
status=response.status
response.read()
conn.close()

# Check that response.status == 200
if status != 200:
    sys.exit(2)

sys.exit(0)
# vim: set ts=4 sw=4 expandtab:
