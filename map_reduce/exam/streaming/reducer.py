#!/usr/bin/python
import sys

last = ''
entries = 0
words = 0
for line in sys.stdin:
    line = line.split()
    person, count = line
    count = int (count)
    if last != person:
        if last != '':
            print("%s\t%d\t%d" % (last, words, entries))
        last = person
        entries = 1
        words = count
    else:
        words += count
        entries += 1

if last != '':
    print("%s\t%d\t%d" % (last, words, entries))

