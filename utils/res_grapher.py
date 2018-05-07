import sys
import pickle
import matplotlib.pyplot as plt

def load_pickle(ps):
    res = []
    for p in ps:
        with open(p, "rb") as f:
            res.append(pickle.load(f))
    return res

ls = load_pickle(sys.argv[1:])
x = [0.0, 0.3, 0.6, 0.9]
dic = dict()
for l in ls:
    for i in l:
        n = i['name']
        aae = i['aae']
        if n in dic:
            dic[n].append(aae)
        else:
            dic[n] = [aae]

names = []
for key in dic:
    plt.plot(x, dic[key])
    names.append(key)

plt.title("Average Absolute Error")
plt.xlabel("Skewness")
plt.ylabel("AAE")
plt.legend(names, loc='upper left')

plt.show()
