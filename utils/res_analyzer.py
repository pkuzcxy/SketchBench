import sys
def read_file(name):
    with open(name) as f:
        res = []
        for line in f:
            res.append(line.split())
        names = res[0][1:]
        data = []
        for line in res[1:]:
            data.append([int(n) for n in line])
        return names, data
    return []

def cdf(arr, thres):
    res = [0 for thr in thres]
    for n in arr:
        for i in range(len(thres)):
            if n <= thres[i]:
                res[i] += 1

    return [r / len(arr) for r in res]

def analyze(r, e):
    ae = [abs(r[i] - e[i]) for i in range(len(r))]
    re = [ae[i] / r[i] for i in range(len(r))]
    aae = (sum(ae) / len(ae))
    are = (sum(re) / len(re))
    acc = (len([aei for aei in ae if aei == 0]) / len(ae))
    ae_cdf = cdf(ae, list(range(10)))
    re_cdf = cdf(re, [n / 10 for n in range(10)])
    return {
        'aae': aae,
        'are': are,
        'acc': acc,
        'ae_cdf': ae_cdf,
        're_cdf': re_cdf,
    }


names, data = read_file(sys.argv[1])
reses = []
for i in range(1, len(names) + 1):
    res = analyze([dat[0] for dat in data], [dat[i] for dat in data])
    res['name'] = names[i-1]
    reses.append(res)

import pickle
with open(sys.argv[2], "wb") as f:
    pickle.dump(reses, f)