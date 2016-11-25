import sys
square = 0
circle = 0

for line in sys.stdin:
    value = int(line)
    square = square + 1
    if value == 1:
        circle = circle + 1
print(4 * circle / square)
