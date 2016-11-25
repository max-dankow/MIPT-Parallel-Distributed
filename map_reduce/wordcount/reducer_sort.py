import sys

for input_line in sys.stdin:
    input_line = input_line.strip()
    key, value = input_line.split("\t", 1)
    print('%s\t%s' % (value, key))
