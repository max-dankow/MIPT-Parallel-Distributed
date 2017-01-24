#!/usr/bin/python
import sys
#import re

for line in sys.stdin:
    #if line == '' or line[0] == '=':
    #    continue
    #line = bad_letters.sub(' ', line)
    line = line.split()
    key = line[0];
    words = line[1::1]
     #print(key,'words', words)
    print( "%s\t%d" % (key.strip(), len(words)))
    #print(1)
