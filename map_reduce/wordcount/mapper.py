import sys
import re

filter_letters_re = re.compile('[^a-zA-Z ]')

for line in sys.stdin:
    line = line.strip()
    line = filter_letters_re.sub(' ', line)
    keys = line.split()
    for key in keys:
        value = 1
        print( "%s\t%d" % (key.strip(), value))
