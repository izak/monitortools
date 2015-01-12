#!/usr/bin/python

import sys
import re

STATUS = re.compile('^[0-9]+ blocks ')
GOOD = re.compile('\[U+\]')

def check():
    lines = open('/proc/mdstat', 'r').readlines()[1:-1]
    lines = [line.strip() for line in lines if len(line.strip())>0]
    bad = 0
    i = 0
    for line in lines:
        if STATUS.match(line):
            if GOOD.search(line) is None:
                bad += 1
    return bad

if __name__ == '__main__':
    try:
        bad = check()
        print "%d drives bad" % bad
    except:
        print "Could not check raid array"
        sys.exit(3)
    else:
        if bad > 0:
            sys.exit(2)
        sys.exit(0)
