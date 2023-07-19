#import ROOT as RT
import numpy as np
import pickle
import sys
import pandas as pd
import os
import glob

def Combine_DF(path, DF_name, Chrg_pkl, DF_com_name):
    DF1 = pd.read_pickle(path+"/AP0000/"+DF_name)
    DF2 = pd.read_pickle(path+"/AP0950/"+DF_name)
    DF3 = pd.read_pickle(path+"/AP1850/"+DF_name)
    DF4 = pd.read_pickle(path+"/AP2750/"+DF_name)
    DF5 = pd.read_pickle(path+"/AP3650/"+DF_name)

    """
    c1 = pd.read_pickle(path+"/AP0000/"+Chrg_pkl)
    c2 = pd.read_pickle(path+"/AP0950/"+Chrg_pkl)
    c3 = pd.read_pickle(path+"/AP1850/"+Chrg_pkl)
    c4 = pd.read_pickle(path+"/AP2750/"+Chrg_pkl)
    c5 = pd.read_pickle(path+"/AP3650/"+Chrg_pkl)
    DF1["chrg"] = c1
    DF2["chrg"] = c2
    DF3["chrg"] = c3
    DF4["chrg"] = c4
    DF5["chrg"] = c5
    """

    t1 = np.array(DF1["seg"])*1000/1024 + 0
    t2 = np.array(DF2["seg"])*1000/1024 + 950
    t3 = np.array(DF3["seg"])*1000/1024 + 1850
    t4 = np.array(DF4["seg"])*1000/1024 + 2750
    t5 = np.array(DF5["seg"])*1000/1024 + 3650

    DF1["time"] = t1
    DF2["time"] = t2
    DF3["time"] = t3
    DF4["time"] = t4
    DF5["time"] = t5


    DF = pd.concat([DF1,DF2,DF3,DF4,DF5],ignore_index=True)
    DF.to_pickle(DF_com_name)

"""
def Combine_DF_K(path, DF_name, Chrg_pkl, Chrg_pkl2, DF_com_name):
    DF1 = pd.read_pickle(path+"/AP0000/"+DF_name)
    DF2 = pd.read_pickle(path+"/AP0950/"+DF_name)
    DF3 = pd.read_pickle(path+"/AP1850/"+DF_name)
    DF4 = pd.read_pickle(path+"/AP2750/"+DF_name)
    DF5 = pd.read_pickle(path+"/AP3650/"+DF_name)


    c1 = pd.read_pickle(path+"/AP0000/"+Chrg_pkl)
    c2 = pd.read_pickle(path+"/AP0950/"+Chrg_pkl)
    c3 = pd.read_pickle(path+"/AP1850/"+Chrg_pkl)
    c4 = pd.read_pickle(path+"/AP2750/"+Chrg_pkl)
    c5 = pd.read_pickle(path+"/AP3650/"+Chrg_pkl)


    c12 = pd.read_pickle(path+"/AP0000/"+Chrg_pkl2)
    c22 = pd.read_pickle(path+"/AP0950/"+Chrg_pkl2)
    c32 = pd.read_pickle(path+"/AP1850/"+Chrg_pkl2)
    c42 = pd.read_pickle(path+"/AP2750/"+Chrg_pkl2)
    c52 = pd.read_pickle(path+"/AP3650/"+Chrg_pkl2)


    DF1["chrg"] = c1
    DF2["chrg"] = c2
    DF3["chrg"] = c3
    DF4["chrg"] = c4
    DF5["chrg"] = c5

    DF1["chrg_ph"] = c12
    DF2["chrg_ph"] = c22
    DF3["chrg_ph"] = c32
    DF4["chrg_ph"] = c42
    DF5["chrg_ph"] = c52

    t1 = np.array(DF1["seg"])*1000/1024 + 0
    t2 = np.array(DF2["seg"])*1000/1024 + 950
    t3 = np.array(DF3["seg"])*1000/1024 + 1850
    t4 = np.array(DF4["seg"])*1000/1024 + 2750
    t5 = np.array(DF5["seg"])*1000/1024 + 3650


    DF1["time"] = t1
    DF2["time"] = t2
    DF3["time"] = t3
    DF4["time"] = t4
    DF5["time"] = t5

    DF = pd.concat([DF1,DF2,DF3,DF4,DF5],ignore_index=True)
    DF.to_pickle(DF_com_name)
"""

def Combine_DF_K_QD(path, DF_name, QD_DF, Chrg_pkl, QD_ch, Chrg_pkl2, QD_ch2, DF_com_name):
    DF1 = pd.read_pickle(path+"/AP0000/"+DF_name)
    DF2 = pd.read_pickle(path+"/AP0950/"+DF_name)
    DF3 = pd.read_pickle(path+"/AP1850/"+DF_name)
    DF4 = pd.read_pickle(path+"/AP2750/"+DF_name)
    DF5 = pd.read_pickle(path+"/AP3650/"+DF_name)

    Empty = pd.DataFrame()

    if os.path.exists(path+"/AP0000/"+QD_DF) == True:
        QDF1 = pd.read_pickle(path+"/AP0000/"+QD_DF)
        """
        Qc1 = pd.read_pickle(path+"/AP0000/"+QD_ch)
        Qc12 = pd.read_pickle(path+"/AP0000/"+QD_ch2)
        QDF1["chrg"] = Qc1
        QDF1["chrg_ph"] = Qc12
        """
        Qt1 = np.array(QDF1["seg"])*1000/1024 + 0
        QDF1["time"] = Qt1

        Empty = pd.concat([Empty,QDF1],ignore_index=True)


    if os.path.exists(path+"/AP0950/"+QD_DF) == True:
        QDF2 = pd.read_pickle(path+"/AP0950/"+QD_DF)
        """
        Qc2 = pd.read_pickle(path+"/AP0950/"+QD_ch)
        Qc22 = pd.read_pickle(path+"/AP0950/"+QD_ch2)
        QDF2["chrg"] = Qc2
        QDF2["chrg_ph"] = Qc22
        """
        Qt2 = np.array(QDF2["seg"])*1000/1024 + 950
        QDF2["time"] = Qt2

        Empty = pd.concat([Empty,QDF2],ignore_index=True)

    if os.path.exists(path+"/AP1850/"+QD_DF) == True:
        QDF3 = pd.read_pickle(path+"/AP1850/"+QD_DF)
        """
        Qc3 = pd.read_pickle(path+"/AP1850/"+QD_ch)
        Qc32 = pd.read_pickle(path+"/AP1850/"+QD_ch2)
        QDF3["chrg"] = Qc3
        QDF3["chrg_ph"] = Qc32
        """
        Qt3 = np.array(QDF3["seg"])*1000/1024 + 1850
        QDF3["time"] = Qt3

        Empty = pd.concat([Empty,QDF3],ignore_index=True)


    if os.path.exists(path+"/AP2750/"+QD_DF) == True:
        QDF4 = pd.read_pickle(path+"/AP2750/"+QD_DF)
        """
        Qc4 = pd.read_pickle(path+"/AP2750/"+QD_ch)
        Qc42 = pd.read_pickle(path+"/AP2750/"+QD_ch2)
        QDF4["chrg"] = Qc4
        QDF4["chrg_ph"] = Qc42
        """
        Qt4 = np.array(QDF4["seg"])*1000/1024 + 2750
        QDF4["time"] = Qt4

        Empty = pd.concat([Empty,QDF4],ignore_index=True)


    if os.path.exists(path+"/AP3650/"+QD_DF) == True:
        QDF5 = pd.read_pickle(path+"/AP3650/"+QD_DF)
        """
        Qc5 = pd.read_pickle(path+"/AP3650/"+QD_ch)
        Qc52 = pd.read_pickle(path+"/AP3650/"+QD_ch2)
        QDF5["chrg"] = Qc5
        QDF5["chrg_ph"] = Qc52
        """
        Qt5 = np.array(QDF5["seg"])*1000/1024 + 3650
        QDF5["time"] = Qt5

        Empty = pd.concat([Empty,QDF5],ignore_index=True)

    """
    c1 = pd.read_pickle(path+"/AP0000/"+Chrg_pkl)
    c2 = pd.read_pickle(path+"/AP0950/"+Chrg_pkl)
    c3 = pd.read_pickle(path+"/AP1850/"+Chrg_pkl)
    c4 = pd.read_pickle(path+"/AP2750/"+Chrg_pkl)
    c5 = pd.read_pickle(path+"/AP3650/"+Chrg_pkl)

    c12 = pd.read_pickle(path+"/AP0000/"+Chrg_pkl2)
    c22 = pd.read_pickle(path+"/AP0950/"+Chrg_pkl2)
    c32 = pd.read_pickle(path+"/AP1850/"+Chrg_pkl2)
    c42 = pd.read_pickle(path+"/AP2750/"+Chrg_pkl2)
    c52 = pd.read_pickle(path+"/AP3650/"+Chrg_pkl2)

    DF1["chrg"] = c1
    DF2["chrg"] = c2
    DF3["chrg"] = c3
    DF4["chrg"] = c4
    DF5["chrg"] = c5

    DF1["chrg_ph"] = c12
    DF2["chrg_ph"] = c22
    DF3["chrg_ph"] = c32
    DF4["chrg_ph"] = c42
    DF5["chrg_ph"] = c52
    """

    t1 = np.array(DF1["seg"])*1000/1024 + 0
    t2 = np.array(DF2["seg"])*1000/1024 + 950
    t3 = np.array(DF3["seg"])*1000/1024 + 1850
    t4 = np.array(DF4["seg"])*1000/1024 + 2750
    t5 = np.array(DF5["seg"])*1000/1024 + 3650

    DF1["time"] = t1
    DF2["time"] = t2
    DF3["time"] = t3
    DF4["time"] = t4
    DF5["time"] = t5

    #DF = pd.concat([DF1,DF2,DF3,DF4,DF5,QDF1,"""QDF2,QDF3,QDF4,QDF5"""],ignore_index=True)
    DF = pd.concat([DF1,DF2,DF3,DF4,DF5,Empty],ignore_index=True)
    DF.to_pickle(DF_com_name)


def Merge_DF(which, path, file_name):
    Dataset = sorted(glob.glob("{0}AP*/*{1}_Cat.pkl".format(path, which)))
    Timing = [0, 950, 1850, 2750, 3650]
    if len(Dataset) == 0:
        print("NO Data")
    else:
        Case = pd.DataFrame()
        for i in range(len(Dataset)):
            print(Dataset[i])
            Df_temp = pd.read_pickle(Dataset[i])
            Df_temp["time"] = np.array(Df_temp["seg"])*1000/1024+Timing[i]
            display(Df_temp)
            Case = pd.concat([Case, Df_temp], ignore_index = True)
        #file_name = Dataset[i].split("/")[-1].split("AP3650")[0] + "{0}_All_Cat.pkl".format(which)
        if os.path.exists("{0}ap_result/".format(path)) == False:
            os.mkdir("{0}ap_result/".format(path))
        Case.to_pickle("{0}ap_result/".format(path)+file_name)

