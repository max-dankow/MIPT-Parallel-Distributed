import sys

for line in sys.stdin:
    key, value = line.split("\t", 1)
    print( "%s\t%s" % (int(value), key.strip()))
