import sys
import os
import random

'''
python3 utils/data_gen.py 12285667 763206 0.0 4 test1.dat
'''
'''
tot = int(sys.argv[1])
dis = int(sys.argv[2])
skew = float(sys.argv[3])
bytePerStr = int(sys.argv[4])
filepath = sys.argv[5]
'''
tot = 12285667
dis = 763206
skew = 0.0
bytePerStr = 4
filepath = "Debugtest5.dat"

def zipf(N, s):
    base = 0.0
    res = []
    for n in range(1, N + 1):
        t = 1 / (n ** s)
        base += t
        res.append(t)
    return [r / base for r in res]

def gen_random_strings(len, byts):
    strs = set()
    res = []
    for i in range(len):
        s = os.urandom(byts)
        while s in strs:
            s = os.urandom(byts)
        res.append(s)
        strs.add(s)
    return res


def gen(fp, freqs, byts):
    strs = gen_random_strings(len(freqs), byts)
    chs = [i for i in range(len(freqs))]
    while len(chs) != 0:
        p = random.randint(0, len(chs) - 1)
        pos = chs[p]
        fp.write(strs[pos])
        freqs[pos] -= 1
        if freqs[pos] == 0:
            del chs[p]


props = zipf(dis, skew)
freq = [round(prop * tot) for prop in props]
with open(filepath, "wb") as f:
    gen(f, freq, bytePerStr)
