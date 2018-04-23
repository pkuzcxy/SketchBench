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
ss = [s[key] for key in s]
ss.sort()
if len(sys.argv) > 3:
    rm = int(sys.argv[3])
    ss = ss[:-rm]

tot = 0
maxFreq = 0
minFreq = 100000
uniq = len(ss)
for sss in ss:
    tot += sss
    maxFreq = max(maxFreq, sss)
    minFreq = min(minFreq, sss)

ax = plt.subplot(111)
plt.hist(ss, 100)
#print([s[key] for key in s])
plt.xlabel('Freq')
plt.ylabel('Num')
#plt.yscale('log')
plt.yscale('log', nonposy='clip')
plt.text(.7, .7, 'max=%d\nmin=%d\nuniq=%d\ntot=%d\nave=%f' % (maxFreq, minFreq, uniq, tot, tot/uniq), transform=ax.transAxes)
plt.grid(True)
plt.xlim(xmin=0)
plt.show()