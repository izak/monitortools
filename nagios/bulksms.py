#!/usr/bin/python
#
# Read message from stdin and send via buksms gateway

import sys, httplib, urllib

username = "user"
password = "pass"

if len(sys.argv)!=2:
    sys.stderr.write("Usage: %s msisdn\n"%sys.argv[0])
    sys.exit(1)

# Read message body from stdin
text = sys.stdin.read()

params = urllib.urlencode({'username':username, 'password':password, 'message':text[:160], 'msisdn':sys.argv[1]})
conn = httplib.HTTPConnection("bulksms.2way.co.za:5567")
conn.request("GET", "/eapi/submission/send_sms/2/2.0?%s" % params)
response = conn.getresponse()
status=response.status
response.read()
conn.close()

# Check that response.status == 200
if status != 200:
    sys.exit(2)

sys.exit(0)
# vim: set ts=4 sw=4 expandtab:
