#!/usr/bin/python
import sys

last = ''
total_entries = 0
total_words = 0
for line in sys.stdin:
    line = line.split()
    if len(line) < 3:
       continue
    total_entries += int(line[2])
    total_words += int(line[1])
if total_entries != 0:
   print(total_words / total_entries)
