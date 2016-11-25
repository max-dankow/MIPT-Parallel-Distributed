import sys

last_key = None
running_total = 0
current_word = None
count = 0;

for input_line in sys.stdin:
    input_line = input_line.strip()
    this_key, value = input_line.split("\t", 1)
    if current_word == this_key:
        count = count + int(value)
    else:
        if current_word:
           print( "%s\t%d" % (current_word, count))
        current_word = this_key
        count = 1

print( "%s\t%d" % (current_word, count))
