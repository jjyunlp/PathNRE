#! /usr/bin/python
# coding=utf8

"""
Designed to count the average sentence num of bags and its distribution
"""

f = open("../data/train.txt", 'r')
#f = open("../../../Data/NYT-2010/train.txt", 'r')
line = f.readline()
pairMap = {}
while(len(line)!=0):
    items = line.split("\t")
    if (len(items) != 7):
        print("line error")
        print line
        continue
    e1=items[0]
    e2=items[1]
    key1 = e1+"\t"+e2
    key2 = e2+"\t"+e1
    if key1 in pairMap:
        pairMap[key1] += 1
    elif key2 in pairMap:
        pairMap[key2] += 1
    else:
        pairMap[key1] = 1
    line = f.readline()

fw = open("pairMap.txt", 'w')
senNum = 0
distribution = [0,0,0,0,0,0,0]
for k,v in pairMap.items():
    if v >= 100:
        senNum += v
        distribution[0] += 1
    elif v >= 50:
        senNum += v
        distribution[1] += 1
    elif v >= 20:
        senNum += v
        distribution[2] += 1
    elif v >= 10:
        senNum += v
        distribution[3] += 1
    elif v >= 5:
        senNum += v
        distribution[4] += 1
    elif v >= 2:
        senNum += v
        distribution[5] += 1
    else:
        senNum += v
        distribution[6] += 1
print distribution
total = 0
for x in distribution:
    total += x
#total = [total+=x for x in distribution]
print total
fw.write(str(distribution)+"\n")
fw.write("total bags: " + str(total))
fw.close()



