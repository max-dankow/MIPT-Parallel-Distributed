import sys

for line in sys.stdin:
    x, y = line.split("\t", 1)
    x, y = float(x), float(y)
    value = 1 if x**2 + y**2 <= 1 else 0
    print(value)
