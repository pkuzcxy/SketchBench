import sys
import os
import pickle
import numpy as np
import math

#analyze ####
def cdf(arr, thres):
    res = [0 for thr in thres]
    for n in arr:
        for i in range(len(thres)):
            if n <= thres[i]:
                res[i] += 1
    return [r / len(arr) for r in res]

def analyze_freq_mre(r, e):
    order = np.argsort(r)
    r = np.array(r)[order] #ascending order
    e = np.array(e)[order] #ascending order
    
    r_distinct = []
    e_distinct = []
    tmp = r[0]
    stt = 0
    for i in range(len(r)):
        if i == len(r) - 1:
            r_distinct.append(r[i])
            e_distinct.append(sum(e[stt : i + 1]) / (i + 1 - stt))
            break
        if r[i + 1] != tmp:
            r_distinct.append(r[i])
            e_distinct.append(sum(e[stt : i + 1]) / (i + 1 - stt))
            tmp = r[i + 1]
            stt = i + 1
 
    sre = [(e_distinct[i] - r_distinct[i]) / r_distinct[i] for i in range(len(r_distinct))] #re with sigh
    re  = [abs(sre[i]) for i in range(len(r_distinct))]
    return {
        'r'   : r_distinct,
        're'  : re,
        'sre' : sre,
    }
    
def analyze_freq_cdf(r, e):
    ae = [abs(r[i] - e[i]) for i in range(len(r))]
    re = [ae[i] / r[i] for i in range(len(r))]
    ae_cdf = cdf(ae, list(range(10)))
    re_cdf = cdf(re, [n / 10 for n in range(10)])
    return {
        'ae-cdf': ae_cdf,
        're-cdf': re_cdf,
    }

def analyze_freq(r, e):    
    ae = [abs(r[i] - e[i]) for i in range(len(r))]
    re = [ae[i] / r[i] for i in range(len(r))]
    aae = (sum(ae) / len(ae))
    are = (sum(re) / len(re))
    acc = (len([aei for aei in ae if aei == 0]) / len(ae))

    return {
        'aae': aae,
        'are': are,
        'acc': acc,
    }

def analyze_topk(rid, rf, eid, ef):
    rid = rid[ : len(eid)]
    recall = len([p for p in rid if p in eid])/ len(rid)
    return {
        'recall': recall,
    }

def analyze_throughput(n, t):
    throughput = n[0] / t[0];
    return {
        'throughput': throughput,
    }
#analyze ####

#read file ####
def read_file(name):
    with open(name) as f:
        res = []
        for line in f:
            res.append(line.split())
        names = res[0]
        data = []
        for line in res[1:]:
            data.append([eval(myeval(n)) for n in line])
        return names, data
    return []
#read file ####
def myeval(n):
    if n=='-nan' or n=='nan':
        return "1"
    else:
        return n

#environment helpers ####
def to_int(tag):
    return int(''.join(filter(str.isdigit, tag)))

def get_env(argv):
    env = dict()
    for arg in argv:
        name, val = tuple(arg.split('='))
        if name in ('hashnum', 'bpc', 'memory', 'topx'):
            val = to_int(val)
        env[name] = val #TODO error and to_float
    return env

def env_to_str(env):
    env_list = []
    for name in env:
        env_list.append(name + "=" + str(env[name]))
    return '*'.join(env_list)
        
def match_env(env, file_env):
    for name in env:
        if name == 'memory':
            if abs(env[name] - file_env[name]) > 10:
                #print(name + 'mismatch')
                return 0
        else:
            if env[name] != file_env[name]:
                #print(name + 'mismatch')
                return 0
    return 1
#environment helpers ####
  
def read_folder_topk(path, key, env):
    names, data = read_file(os.path.join(path, 'top5000_exactRank_' + env['data'] + '.txt'))
    rid, rf = [dat[0] for dat in data], [dat[1] for dat in data]

    files = os.listdir(path)
    reses = []
    throughputs = []
    for fn in files:
        if os.path.isdir(fn): continue
        if not fn.startswith('top'): continue
        if 'spaceSaving' in fn: continue #TODO
        if 'webdocs' in fn: continue #TODO
        if 'exactRank' in fn: continue
        if 'pickle' in fn: continue
        
        #print(tuple(fn.split('_')))
        topx, type, sketch, skewness, hashnum, bitpercounter, counterperarray = tuple(fn.split('_'))
        if type == 'throughput': continue #compute throughput along with result
        counterperarray = counterperarray.split('.')[0]
        file_env = {
            'topx'    : to_int(topx),
            'data'    : skewness,
            'hashnum' : to_int(hashnum),
            'bpc'     : to_int(bitpercounter),
            'memory'  : to_int(hashnum) * to_int(counterperarray),
        }
        if not match_env(env, file_env): continue
        #read result file
        names, data = read_file(os.path.join(path, fn))
        d0, d1 = [dat[0] for dat in data], [dat[1] for dat in data]
        res = {
            'sketch' : sketch,
            'env'    : file_env,
            key      : file_env[key],
        }
        res.update(analyze_topk(rid, rf, d0, d1))
        reses.append(res);        
        #read throughput file
        fn = '_'.join([topx, 'throughput', sketch, skewness, hashnum, bitpercounter.lower(), (counterperarray + '.txt').lower()])
        names, data = read_file(os.path.join(path, fn))
        d0, d1 = [dat[0] for dat in data], [dat[1] for dat in data]
        res.update(analyze_throughput(d0, d1))
        
        throughputs.append(res);

    #dump all result
    with open(path + '/topk_' + key + '_' + env_to_str(env) + '.pickle', "wb") as f:
        pickle.dump(reses, f)        

def read_file_freq(path, fn, key, env):
    freqstr, type, sketch, skewness, hashnum, bitpercounter, counterperarray = tuple(fn.split('_'))
    counterperarray = counterperarray.split('.')[0] #remove .txt
    file_env = {
        'data'    : skewness,
        'hashnum' : to_int(hashnum),
        'bpc'     : to_int(bitpercounter),
        'memory'  : to_int(hashnum) * to_int(counterperarray),
    }
    if not match_env(env, file_env): return None, None
    
    names, data = read_file(os.path.join(path, fn))
    d1, d2 = [dat[0] for dat in data], [dat[1] for dat in data]
    
    freq_res = None
    freq_throughput = None
    if type == 'result':
        if key == 'cdf':
            freq_res = analyze_freq_cdf(d1, d2)
        elif key == 'mre':
            freq_res = analyze_mre_cdf(d1, d2)
        else:
            freq_res = analyze_freq(d1, d2)
            freq_res[key] = file_env[key]
        freq_res['sketch'] = sketch
        freq_res['env'] = env
    elif type == 'throughput':
        freq_throughput = analyze_freq(d1, d2)
        freq_throughput['sketch'] = sketch
        freq_throughput['env'] = env
    
    return freq_res, freq_throughput

def read_folder_freq(path, key, env):
    files = os.listdir(path)
    reses = []
    throughputs = []
    for fn in files:
        if os.path.isdir(os.path.join(path,fn)): continue
        if fn.startswith('frequency_'):
            res, freq = read_file_freq(path, fn, key, env)
            if res != None:
                reses.append(res)
            if freq != None:
                throughputs.append(res)
    with open(path + '/freq_reses' + '_' + key + '_' + env_to_str(env) + '.pickle', "wb") as f:
        pickle.dump(reses, f)
    with open(path + '/freq_throughputs' + '_' + key + '_' + env_to_str(env) + '.pickle', "wb") as f:
        pickle.dump(throughputs, f)
            
def usage(argv):
    print(argv[0] + ' --help')
    print('--freq  <skewness|hashnum|MemorySize|throughput|cdf|mre>  <data_folder_name>  <environment>...')
    print('--topk  <skewness|hashnum|MemorySize|throughput|>         <data_folder_name>  <file_name_pattern>...')





def topk_analyzer(env):
    sketch_name = ['spaceSaving','ass']
    names, trueData = read_file(os.path.join(env['path'], 'top5000_exactRank_' + env['dataset'] + '.txt'))
    i  = 1
    trueRank ={}
    trueFreq ={}
    for line in trueData:
        trueRank[line[0]] = i
        trueFreq[line[0]] = line[1]
        i += 1
    res = dict()
    for i in range(len(sketch_name)):
        res[sketch_name[i]] = dict()
        k = 8
        while(True):
            recall = 0
            rank2 = 0.0
            re =0.0
            ae = 0.0
            if sketch_name[i]=='sbfsketch':
                topk_file_name ="top"+str(k)+"_result_"+sketch_name[i]+\
                                "_"+env['dataset']+\
                                "_hashnum"+env['hashnum']+\
                                "_bitPerCounter24"+\
                                "_counterPerArray"+str(int(env['counternum'])*int(env['hashnum']))
            elif sketch_name[i]=='spaceSaving':
                topk_file_name =  "top"+str(k)+"_result_"+sketch_name[i]+\
                                "_"+env['dataset']
            elif sketch_name[i]=='ass':
                topk_file_name =  "top"+str(k)+"_result_"+sketch_name[i]+\
                                "_"+env['dataset']
            else:
                topk_file_name = "top"+str(k)+"_result_"+sketch_name[i]+\
                                "_"+env['dataset']+\
                                "_hashnum"+env['hashnum']+ \
                                "_bitPerCounter24"+ \
                                "_counterPerArray"+ env['counternum']
            name, data = read_file(os.path.join(env['path'], topk_file_name+ '.txt'))
            j = 1
            for line in data:
                if line[0] in trueRank.keys():
                    if trueRank[line[0]] <= k:
                        rank2 += (j-trueRank[line[0]])*(j-trueRank[line[0]])
                        recall += 1
                        j+=1
                        re += abs(line[1]-trueFreq[line[0]])/float(trueFreq[line[0]])
                        ae += abs(line[1]-trueFreq[line[0]])
            res[sketch_name[i]][k] = dict()
            res[sketch_name[i]][k]['recall'] = float(recall)/k
            res[sketch_name[i]][k]['are'] = re/recall
            res[sketch_name[i]][k]['aae'] = ae/recall
            '''
            print(sketch_name[i]+"are:"+str(res[sketch_name[i]][k]['are'])+"\n")
            print(sketch_name[i]+"aae:"+str(res[sketch_name[i]][k]['aae'])+"\n")
            '''
            if sketch_name[i] != 'ass':
                print(sketch_name[i]+"top"+str(k)+":"+str(res[sketch_name[i]][k]['recall'])+"\n")
            else :
                print('UNNAMED'+"top"+str(k)+":"+str(res[sketch_name[i]][k]['recall'])+"\n")
            if recall!=0 and recall!=1:
                res[sketch_name[i]][k]['p'] = 1- 6*rank2/recall/(recall-1)/(recall+1)
            else:
                res[sketch_name[i]][k]['p'] = 0
            '''
            print(sketch_name[i]+":"+str(res[sketch_name[i]][k]['p'])+"\n")
            '''
            k = k * 2
            if k== 1024*2:
                break
    pickleName ="PickleTopk_"+env['dataset']\
                                 +"_hashnum"+env['hashnum']+"_bitPerCounter24"+\
                                 "_counterPerArray"+env['hashnum']
    with open(env['path']+pickleName+ '.pickle', "wb") as f:
        pickle.dump(res, f)


def heavyChange_analyzer(env):
    sketch_name = ['asketch','cmmsketch','cmmcusketch','cmsketch','csketch','csmsketch','cusketch','Lcusketch','sbfsketch']
    res =dict()
    for i in range(len(sketch_name)):
        res[sketch_name[i]]  = dict()
        recall = 0.0
        precision = 0.0
        if sketch_name[i]=='sbfsketch':
            heavyChange_file_name =  "heavyChange_"+sketch_name[i]+"_"+env['dataset']\
                                 +"_hashnum"+env['hashnum']+"_bitPerCounter24"+\
                                 "_counterPerArray"+str(int(env['counternum'])*int(env['hashnum']))
        else:
             heavyChange_file_name =  "heavyChange_"+sketch_name[i]+"_"+env['dataset']\
                                 +"_hashnum"+env['hashnum']+"_bitPerCounter24"+\
                                 "_counterPerArray"+env['counternum']
        names, data = read_file(os.path.join(env['path'], heavyChange_file_name+ '.txt'))
        for line in data:
            if float(line[names.index('recall')]) !=float('-nan') and float(line[names.index('recall')]) !=float('nan')  :
                recall += float(line[names.index('recall')])
            if (float(line[names.index('trueReport')])+float(line[names.index('falseReport')])) > 0:
                precision += float(line[names.index('trueReport')]) \
                         / (float(line[names.index('trueReport')])+float(line[names.index('falseReport')]))
        res[sketch_name[i]]['recall'] = recall/len(data)
        res[sketch_name[i]]['precision'] = precision/len(data)
        print(sketch_name[i]+str(res[sketch_name[i]]['recall'])+"\n")
        print(sketch_name[i]+str(res[sketch_name[i]]['precision'])+"\n")
    pickleName ="PickleheavyChange_"+env['dataset']\
                                 +"_hashnum"+env['hashnum']+"_bitPerCounter24"+\
                                 "_counterPerArray"+env['hashnum']
    with open(env['path']+pickleName+ '.pickle', "wb") as f:
        pickle.dump(res, f)

def parse_commandline(type,env):
    if type == 'topk':
        topk_analyzer(env)
    elif type == 'frequency':
        pass
        #TODO
    elif type == 'heavyChange':
        heavyChange_analyzer(env)
def main(argv):
    '''
    -hc -d kosarak.dat -hashnum 3 -counternum 3883 -p ./result
    '''
    i = 1
    type = "" #task_type:topk\frequency\heacyChange
    env = dict() #environment:dataset
    while i < len(argv):
        arg = argv[i]
        if arg in ('--help', '-h'):
            usage(argv)
        elif arg in ('--topk', '-t'):
            '''
            while i + 2 < len(argv) and not argv[i + 1].startswith('-'):
                x_key = argv[i + 1]
                path_name = argv[i + 2]
                if x_key in ('skewness', 'hashnum', 'm'):
                    topk_env = get_env(argv[i + 3 : i + 7])             
                    i += 7
                else:
                    usage(argv);
                    exit(2);
                read_folder_topk(path_name, x_key, topk_env)
            '''
            type = "topk"
        elif arg in ('--freq', '-f'):
            '''
            while i + 2 < len(argv) and not argv[i + 1].startswith('-'):
                x_key = argv[i + 1]
                path_name = argv[i + 2]
                if x_key in ('skewness', 'hashnum', 'memory'):
                    #TODO error
                    freq_env = get_env(argv[i + 3 : i + 6])
                    i += 6
                elif x_key == 'cdf':
                    freq_env = get_env(argv[i + 3 : i + 7])
                    i += 7                   
                elif x_key == 'mre':
                    freq_env = get_env(argv[i + 3 : i + 7])
                    i += 7                   
                else:
                    usage(argv);
                    exit(2);
                read_folder_freq(path_name, x_key, freq_env)
            '''
            type = "frequency"
        elif arg in ('-hc'):
            type = "heavyChange"
        elif arg in ('-d'):
            i += 1
            env['dataset'] =  argv[i]
        elif arg in ('-p'):
            i += 1
            env['path'] =  argv[i]
        elif arg in ('-hashnum'):
            i += 1
            env['hashnum'] =  argv[i]
        elif arg in ('-counternum'):
            i += 1
            env['counternum'] = argv[i]
        else:
            usage(argv);
            exit(2);
        i += 1
    parse_commandline(type,env)
main(sys.argv)