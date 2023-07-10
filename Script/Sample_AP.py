import pymodule.IO.Convert_DRS4 as DRS4
import pymodule.Avg_wf.Calc_Avg_Wf as AVG
import pymodule.Charge_analysis.Charge_Dist as Chrg
import pymodule.Fitting.Fitting as Fit
import pymodule.AP_analysis as AP
import sys
import pandas as pd
import os
import subprocess
import ROOT as RT
import numpy as np
import pickle
import glob
import matplotlib.pyplot as plt


def Check_Tr(file_o):
    file = RT.TFile(file_o, "READ")
    tree_names = [key.GetName() for key in file.GetListOfKeys() if key.GetClassName() == "TTree"]
    bo = "Treesingle_0" in tree_names
    bo2 = "Treesource_0" in tree_names
    if bo == True:
        trr = "Treesingle_0"
        print(trr)
    elif bo2 == True:
        trr = "Treesource_0"
        print(trr)
    else:
        trr ="ERROR!"
        print(trr)
    return trr
    
def AP_counts(work_dir, file_o, tr):
    tr_d = "Treedark_0"
    new_file = "new.root"
    diff_file = "diff.root"
    file_wf = "wform.root"
    avg_s_pkl = "avg_s.pkl"
    avg_d_pkl = "avg_d.pkl"
    N_pd = "N_ap.pkl"
    S_pd = "S_ap.pkl"
    K_pd = "K_ap.pkl"

    create = DRS4.Create_ROOT_file_2386(file_o, file_o, work_dir, work_dir+new_file, 10, tr, tr_d)
    avg_wf = AVG.Calc_Avg_Wf_all(work_dir+new_file, work_dir+new_file, work_dir+avg_s_pkl, work_dir+avg_d_pkl, tr_s  = tr)
    AP.Generate_DIFF_File(work_dir+new_file, work_dir+diff_file, work_dir+avg_d_pkl, tree_s = tr)
    thres = AP.Define_DIFF_THRES(work_dir+diff_file)
    AP.Counts_AP_N(work_dir+diff_file, work_dir+N_pd, thres, 60, 1000)

    AP.Generate_WFORM_File(work_dir+new_file, work_dir+file_wf, work_dir+avg_s_pkl, tree_s = tr)
    AP.Counts_AP_S(work_dir+file_wf, S_pd, 10, 3, 60, 1000, work_dir+avg_s_pkl)
    AP.Counts_AP_K(work_dir+diff_file, work_dir+file_wf,  5, 10, work_dir+N_pd, work_dir+K_pd)


if __name__ == '__main__':

    new_dir_path = "/Users/kiyomoto/reaserch/230710/"
    
    for name in glob.glob('/Users/kiyomoto/reaserch/PMT_data/pmt/*/*/*AP*.root'):
        file_path = name
        Timing = name.split(".root")[0].split("_")[-1]
        PMT = name.split(".root")[0].split("_")[-2]
        dir_path = new_dir_path + PMT + "/"+Timing+"/"
        os.mkdir(dir_path)
        print(file_path)
        hh = Check_Tr(file_path)
        if hh =="ERROR!":
            #print(file_path)
            continue
        AP_counts(dir_path, file_path, hh)
        #print(PMT)
        #print(Timing)
    """
    file = "/Users/kiyomoto/reaserch/PMT_data/pmt/230627/AC1952/20230627_AC1952_AP0000.root"
    hh = Check_Tr(file)
    if hh != "ERROR!":
        AP_counts(new_dir_path, file, hh)
    """