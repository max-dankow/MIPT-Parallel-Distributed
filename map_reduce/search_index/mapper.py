import sys
import re

term = 'term'

for line in sys.stdin:
    keys = line.split()
    article_id = keys[0]
    for key in keys[1::1]:
        value = 1
        print( "%s\t%d" % (key.strip(), value))
