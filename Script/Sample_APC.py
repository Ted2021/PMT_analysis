import PMT_analyser.Charge_analysis.Charge_AP as CAP
import ROOT as RT
import numpy as np
import glob
import os
import pickle 
import sys
import pandas as pd

def Return_Charge_nparray(path):
    file = "wform.root"
    tr = "Treesource_0"
    avf = "avg_d.pkl"
    
    n_df = "N_ap.pkl" 
    s_df = "S_ap.pkl"
    k_df = "K_ap.pkl"
    N_chrg = "N_ch.pkl"
    S_chrg = "S_ch.pkl"
    K_chrg = "K_ch_t2.pkl"
    
    print('### input {0} ###'.format(path))

    #CAP.Calc_AP_Charge(path+file, tr, path+avf, path+n_df, path+N_chrg)
    #CAP.Calc_AP_Charge(path+file, tr, path+avf, path+s_df, path+S_chrg)
    CAP.Calc_AP_Charge_K(path+file, tr, path+avf, path+k_df, path+K_chrg)
    print('### PMT: {0}, TIME: {1}, DONE! ###'.format(path.split("/")[-2], path.split("/")[-1]))

if __name__ == '__main__':

    #new_dir_path = "/Users/kiyomoto/reaserch/230710/"
    #new_dir_path = "/Users/kiyomoto/reaserch/230710/hoge/"
    
    """
    for name in glob.glob('/Users/kiyomoto/reaserch/PMT_data/pmt/*/*/*AP*.root'):
        file_path = name
        Timing = name.split(".root")[0].split("_")[-1]
        PMT = name.split(".root")[0].split("_")[-2]
        dir_path = new_dir_path + PMT + "/"+Timing+"/"
        #os.mkdir(dir_path)
        print(file_path)
        hh = Check_Tr(file_path)
        if hh =="ERROR!":
            #print(file_path)
            continue
        AP_counts(dir_path, file_path, hh)
        #print(PMT)
        #print(Timing)
    """
    """
    for name in glob.glob("/Users/kiyomoto/reaserch/230710/A*/AP*/"):
        file_n = name
        Return_Charge_nparray(file_n)
    """
    #file = "/Users/kiyomoto/reaserch/230710/AC2815/AP2750/"
    file = "/Users/kiyomoto/reaserch/230710/AC1949/AP0000/"
    Return_Charge_nparray(file)
    