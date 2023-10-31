import pandas as pd
import sys
dataFileName = '../out/0_output.txt'


def DataLoaderAll(fileName =None):
    dataFileName2 = dataFileName
    if fileName !=None: dataFileName2 = './data/'+fileName

    df = pd.read_csv(dataFileName2,sep='\t',header=None )

    return df
def DataLoaderByKey(key = "FOUND_AN_ECHONET_ENDPOINT", fileName = None):
    dataFileName2 = dataFileName
    if fileName !=None: dataFileName2 = './data/'+fileName

    df = pd.read_csv(dataFileName2,sep='\t',header=None )
    df2 = df.loc[df[1] == key]
    return df2




def main():
    aa = DataLoaderByKey("FOUND_AN_ECHONET_ENDPOINT")
    print(aa)
    # df = pd.read_csv('../../out/0_output.txt',sep='\t',header=None )  
    # #print(df)
    # key = "FOUND_AN_ECHONET_ENDPOINT"
    # df2 = df.loc[df[1] == key]
    
    
    # print(df2)
    # labels  = df2[2]
    # print(labels)
    # times = df2[0]
    # print(times)
if __name__ == "__main__":
    main()
