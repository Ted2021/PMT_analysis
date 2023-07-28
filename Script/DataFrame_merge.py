#import PMT_analyser.IO.Convert_DRS4 as DRS4
#import PMT_analyser.Avg_wf.Calc_Avg_Wf as AVG
#import PMT_analyser.Charge_analysis.Charge_Dist as Chrg
#import PMT_analyser.Charge_analysis.Charge_AP as CAP
#import PMT_analyser.Fitting.Fitting as Fit
#import PMT_analyser.AP_analysis as AP
import sys
import pandas as pd
import os
import subprocess
import ROOT as RT
import numpy as np
import pickle
import glob
import matplotlib.pyplot as plt
import datetime

def Merge_file(hoge, label):
    Case = pd.DataFrame()
    for i in range(len(hoge)):
        Df_temp = pd.read_pickle(hoge["Path"][i]+hoge[label][i])
        Df_temp["time"] = np.array(Df_temp["seg"])*1000/1024+int(hoge["Path"][i].split("/")[-2].split("AP")[-1])
        display(Df_temp)
        Case = pd.concat([Case, Df_temp], ignore_index = True)
    return Case


if __name__ == '__main__':

    args = sys.argv

    hoge = pd.read_csv(args[1])

    N_all = Merge_file(hoge, "N_Cat")
    S_all = Merge_file(hoge, "S_Cat")
    K_all = Merge_file(hoge, "K_Cat")

    path = hoge["Path"][0].split("AP0000")[0]
    if os.path.exists("{0}ap_result/".format(path)) == False:
        os.mkdir("{0}ap_result/".format(path))
    
    N_all.to_pickle("{0}ap_result/".format(path)+"20230723_N_Cat_All.pkl")
    S_all.to_pickle("{0}ap_result/".format(path)+"20230723_S_Cat_All.pkl")
    K_all.to_pickle("{0}ap_result/".format(path)+"20230723_K_Cat_All.pkl")

