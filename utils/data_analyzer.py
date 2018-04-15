import sys
import matplotlib.pyplot as plt

def file2dict(bytesNum, filePath):
    with open(filePath, "rb") as f:
        s = dict()
        st = f.read(bytesNum)
        while st:
            s[st] = s.get(st, 0) + 1
            st = f.read(bytesNum)
        return s

s = file2dict(int(sys.argv[1]), sys.argv[2])
tot = 0
maxFreq = 0
minFreq = 100000
uniq = len(s)
for key in s:
    tot += s[key]
    maxFreq = max(maxFreq, s[key])
    minFreq = min(minFreq, s[key])
ax = plt.subplot(111)
plt.hist([s[key] for key in s], 30)
plt.xlabel('Freq')
plt.ylabel('Num')
plt.text(.7, .7, 'max=%d\nmin=%d\nuniq=%d\ntot=%d\nave=%f' % (maxFreq, minFreq, uniq, tot, tot/uniq), transform=ax.transAxes)
plt.grid(True)
plt.xlim(xmin=0)
plt.show()