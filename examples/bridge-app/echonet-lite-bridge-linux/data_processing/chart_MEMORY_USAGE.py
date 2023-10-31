from toansttlib import *
import pandas as pd
import os
import matplotlib.pyplot as plt
import numpy as np
import shutil
from datatool.DataLoader import DataLoaderByKey
import sys
from toansttlib import *

dataTime = []
dataMemHeap = []
dataMemHeapExtra = []
dataStack = []

with open('./data/massif.out.22082') as f:
    while True:
        line = f.readline()
        if not line:
            break
        line = line.replace("\n","").replace("\r","")
        print(line)
        if line[0:14] == 'mem_heap_extra':
            val = line.split('=')[1]
            dataMemHeapExtra.append(float(val))
        elif line[0:8] == 'mem_heap':
            val = line.split('=')[1]
            dataMemHeap.append(float(val))
        elif line[0:5] == 'time=':
            
            val = line.split('=')[1]
            dataTime.append(float(val))
        elif line[0:10] == 'mem_stacks':
            val = line.split('=')[1]
            dataStack.append(float(val))




print(len(dataTime), np.array(dataTime).shape)
print(len(dataMemHeap), np.array(dataMemHeap).shape)
print(len(dataMemHeapExtra))
print(len(dataStack))
dataTime = np.array(dataTime)
dataTime = dataTime/1000
dataMemHeap = np.array(dataMemHeap)
dataMemHeap = dataMemHeap/(1024)
dataMemHeapExtra = np.array(dataMemHeapExtra)
dataMemHeapExtra = dataMemHeapExtra/(1024)
dataStack = np.array(dataStack)
dataStack = dataStack/(1024)

fig, axs = plt.subplots(1,1, figsize=(8, 6))
fmta(-1)

DrawSubFigure_SimpleLine(axs,dataTime,[dataMemHeap,dataMemHeapExtra,dataStack]  ,x_title='Time (seconds)',y_title='Memory usage (kB)')

datasetIds = ['Heap','Heap Extra','Stack']
axs.legend(datasetIds,ncol=1)
plt.title('Bridged-app memory usage over time ')

fig.tight_layout()

fileName = './Figures/'+os.path.basename(__file__).replace(".py",".pdf")
plt.savefig(fileName)
fileName = './Figures/'+os.path.basename(__file__).replace(".py",".png")
plt.savefig(fileName)
fileName = './Figures/'+os.path.basename(__file__).replace(".py",".svg")
plt.savefig(fileName)
plt.show()
