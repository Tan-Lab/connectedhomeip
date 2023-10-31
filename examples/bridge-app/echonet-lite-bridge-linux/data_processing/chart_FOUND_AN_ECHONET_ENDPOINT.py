from toansttlib import *
import pandas as pd
import os
import matplotlib.pyplot as plt
import numpy as np
import shutil
from datatool.DataLoader import DataLoaderByKey





#params = { "text.usetex" : True,"font.family" : "serif", "font.serif" : ["Computer Modern Serif"]}

df = DataLoaderByKey('FOUND_AN_ECHONET_ENDPOINT',fileName = os.path.basename(__file__).replace(".py",".txt"))
print(df)
dataDict={}

for index, row in df.iterrows():
    asd=123
    key = row[2][0:6]
    timeVal = float(row[0])/1000000
    if key in dataDict:
        dataDict[key].append(timeVal)
    else: dataDict[key] = [timeVal]

x = []
y = []
tstd = []
error =[]
i=0
labels =[]
for key,arr in dataDict.items():
    labels.append(key)
    x.append(i)
    i+=1
    y.append(np.average(arr))
    tstd.append(np.std(arr))
    error.append([np.min(arr), np.max(arr)     ])
print(labels)





params = { "text.usetex" : False,"font.family" : "serif"}
plt.rcParams.update(params)
fig = plt.figure(figsize = (8, 6))



ax= plt.bar(x, y)


for i in range(len(x)):
    plt.text(x[i],y[i] + 0.02, str( "%.2f" % error[i][0]) + '\n~\n'  + str("%.2f" %error[i][1]) , ha= 'center')

plt.xticks(np.arange(len(x)), labels,rotation=90)
plt.xlabel(' Echonet device type')
plt.ylabel(' Time (seconds)')
plt.title('Average discovery time for different echonetLITE device types')
ax = plt.gca()
ax.set_ylim([0, np.max(y)*1.15   ])


fig.tight_layout()

fileName = './Figures/'+os.path.basename(__file__).replace(".py",".pdf")
plt.savefig(fileName)
fileName = './Figures/'+os.path.basename(__file__).replace(".py",".png")
plt.savefig(fileName)
fileName = './Figures/'+os.path.basename(__file__).replace(".py",".svg")
plt.savefig(fileName)
#shutil.copyfile(fileName,fileName2 )

plt.show()
