from toansttlib import *
import pandas as pd
import os
import matplotlib.pyplot as plt
import numpy as np
import shutil
from datatool.DataLoader import DataLoaderByKey





#params = { "text.usetex" : True,"font.family" : "serif", "font.serif" : ["Computer Modern Serif"]}

df = DataLoaderByKey('STARTUP_COUNT_DEVICE',fileName = os.path.basename(__file__).replace(".py",".txt"))
print(df)

x = []
y = []
yi =1
for index, row in df.iterrows():
    x.append(float(row[0])/1000000 )
    y.append(yi)
    yi+=1
print(x)
params = { "text.usetex" : False,"font.family" : "serif"}
plt.rcParams.update(params)
fig = plt.figure(figsize = (8, 6) )
ax= plt.plot(x, y, linewidth=3)


# for i in range(len(x)):
#     plt.text(x[i],y[i] + 0.02, str( "%.2f" % error[i][0]) + '\n~\n'  + str("%.2f" %error[i][1]) , ha= 'center')

# plt.xticks(np.arange(len(x)), labels,rotation=90)
plt.ylabel(' Number of added EchonetLITE devices')
plt.xlabel(' Time (seconds)')
plt.title('Time to add EchonetLITE devices')
# ax = plt.gca()
# ax.set_ylim([0, np.max(y)*1.15   ])


fig.tight_layout()

fileName = './Figures/'+os.path.basename(__file__).replace(".py",".pdf")
plt.savefig(fileName)
fileName = './Figures/'+os.path.basename(__file__).replace(".py",".png")
plt.savefig(fileName)
fileName = './Figures/'+os.path.basename(__file__).replace(".py",".svg")
plt.savefig(fileName)
#shutil.copyfile(fileName,fileName2 )

plt.show()
