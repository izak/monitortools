#!/usr/bin/python

import sys
import re

GOOD = re.compile('\[U+\]')

def check():
    lines = open('/proc/mdstat', 'r').readlines()[1:-1]
    lines = [line.strip() for line in lines if len(line.strip())>0]
    assert len(lines)%2==0, "Something wrong with mdstat"
    bad = 0
    i = 0
    while i < len(lines):
        if GOOD.search(lines[i+1]) is None:
            bad += 1
        i += 2
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
