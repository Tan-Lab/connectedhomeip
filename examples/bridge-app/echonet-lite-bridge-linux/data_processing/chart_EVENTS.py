from toansttlib import *
import pandas as pd
import os
import matplotlib.pyplot as plt
import numpy as np
import shutil
from datatool.DataLoader import DataLoaderAll





#params = { "text.usetex" : True,"font.family" : "serif", "font.serif" : ["Computer Modern Serif"]}


supportedKeys =  ['FOUND_AN_ECHONET_ENDPOINT', 'GET_A_ENDPOINT_INFO','PROCESSED_AN_ECHONET_ENDPOINT', 'RECEIVE_ECHONET_PROPERTY_VALUE', 'STARTUP_COUNT_DEVICE' ]



df = DataLoaderAll(fileName = os.path.basename(__file__).replace(".py",".txt"))

dataDict={}

colors1 = [f'C{i}' for i in range(5)]
lineoffsets1 = [1, 2, 3, 4, 5]
linelengths1 = [0.8, 0.8, 0.8, 0.8, 0.8 ]
data1 = np.random.random([5, 50])

for index, row in df.iterrows():
    key = row[1]
    if key in supportedKeys:
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
data2 = []
for key,arr in dataDict.items():
    print(key)
    labels.append(key)
    x.append(i)
    i+=1
    y.append(np.average(arr))
    tstd.append(np.std(arr))
    error.append([np.min(arr), np.max(arr)     ])
    data2.append(arr)
print(labels)






params = { "text.usetex" : False,"font.family" : "serif"}
plt.rcParams.update(params)
fig = plt.figure(figsize = (8, 6))

print(len(data2), len(lineoffsets1))
plt.eventplot(data2, colors=colors1, lineoffsets=lineoffsets1,
                    linelengths=linelengths1)

#ax= plt.bar(x, y)


#for i in range(len(x)):
 #   plt.text(x[i],y[i] + 0.02, str( "%.2f" % error[i][0]) + '\n~\n'  + str("%.2f" %error[i][1]) , ha= 'center')
supportedKeys =  ['FOUND_AN_ECHONET_ENDPOINT', 'GET_A_ENDPOINT_INFO','PROCESSED_AN_ECHONET_ENDPOINT', 'RECEIVE_ECHONET_PROPERTY_VALUE', 'STARTUP_COUNT_DEVICE' ]


labels = ['Discover', 'Get info', 'Filter data', 'Receive data','Matter Register' ]
plt.yticks(np.arange(1,len(data2)+1), labels)
plt.xlabel(' Time (seconds)')
#plt.ylabel(' Time (seconds)')

plt.title('Briged-app startup events')
ax = plt.gca()
#ax.set_ylim([0, np.max(y)*1.15   ])


fig.tight_layout()

fileName = './Figures/'+os.path.basename(__file__).replace(".py",".pdf")
plt.savefig(fileName)
fileName = './Figures/'+os.path.basename(__file__).replace(".py",".png")
plt.savefig(fileName)
fileName = './Figures/'+os.path.basename(__file__).replace(".py",".svg")
plt.savefig(fileName)
#shutil.copyfile(fileName,fileName2 )

plt.show()
