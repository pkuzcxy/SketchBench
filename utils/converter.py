import sys

with open(sys.argv[1], "r") as f:
    with open(sys.argv[2], "wb") as ff:
        for line in f:
            for num in [int(n) for n in line.split()]:
                ff.write(num.to_bytes(4, byteorder='big'))
