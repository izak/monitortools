#!/usr/bin/python
#
# $Id$
#

""" Add an init script for a zope cluster
"""

import sys
import getopt
import commands

from optparse import OptionParser
parser = OptionParser()
parser.add_option("-d", "--base", help="Base directory where zope cluster lives")
parser.add_option("-D", "--description", help="Description of the instance")
parser.add_option("-t", "--timeout", help="How long to wait for a client to restart", default=60)
parser.add_option("-z", "--zeo", help="Path of zeo relative to base directory", default="zeo")
parser.add_option("-c", "--clients", help="Path of zeo relative to base directory", default="client1 client2")
parser.add_option("-u", "--user", help="What user to run zope as", default="zope")

(options, args) = parser.parse_args()

def check_required(attr):
    if getattr(options, attr, None) is None:
        parser.error("%s is required" % attr)
        sys.exit(1)

check_required('base')
check_required('description')

fp = open("/etc/init.d/zope_%s"%(options.description), "w")
#fp = open("zope_%s"%(options.description), "w")
fp.write("ZOPE=%s\n"%(options.base))
fp.write("DESC=%s\n"%(options.description))
fp.write("ZOPE_RESTART_TIMEOUT=%s\n"%(options.timeout))
fp.write("ZEO=%s\n"%(options.zeo))
fp.write("CLIENTS=\"%s\"\n"%(options.clients))
fp.write("SRVUSER=%s\n\n"%(options.user))
fp.write("source /usr/share/upfrontmonitortools/zope/zope.init")
fp.close()
