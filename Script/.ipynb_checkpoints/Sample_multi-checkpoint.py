import PMT_analyser.IO.Convert_DRS4 as DRS4
import PMT_analyser.Avg_wf.Calc_Avg_Wf as AVG
import PMT_analyser.Charge_analysis.Charge_Dist as Chrg
import PMT_analyser.Fitting.Fitting as Fit
import sys
import pandas as pd
import os
import subprocess
import ROOT as RT
import numpy as np
import pickle
import matplotlib.pyplot as plt

if __name__ == '__main__':
    args = sys.argv
    #input 
    file = args[1]  #解析するMultiのファイル(Source)
    dfile = file    #解析するMultiのファイル(Dark)
    #tr = "Treesource_0" 
    tr = "Treemulti_0"
    tr_d = "Treedark_0"

    #work dir
    work_dir = args[2]

    #Output
    new_file = "new_multi.root"
    avg_s_pkl = "avg_s_multi.pkl"
    avg_d_pkl = "avg_d_multi.pkl"
    avg_fig = "Avg_multi.png"
    chrg_pkl = "chrg_multi.pkl"
    chrg_fig = "Chrg_multi.png"
    histX_pkl = "histX_.pkl"
    histY_pkl = "histY_.pkl"
    para_pkl = "para_multi.pkl"

    """
    create = DRS4.Create_ROOT_file_2385(file, file, work_dir, work_dir+new_file, 32, tr, tr_d)
    avg_wf = AVG.Calc_Avg_Wf_all(work_dir+new_file, work_dir+new_file, work_dir+avg_s_pkl, work_dir+avg_d_pkl, tr, tr_d)
    int_max  = AVG.Plot_Avg(avg_wf[1], avg_wf[2], work_dir+avg_fig)

    charge = Chrg.Calc_Charge(work_dir+new_file, work_dir+new_file, int_max - 20, int_max + 50, work_dir+chrg_pkl, tr, tr_d)
    min_c = int(np.min(np.array(charge)))
    max_c = int(np.max(np.array(charge)))
    hist = Chrg.Plot_Hist(charge, min_c-10, max_c+10)

    p = Fit.Find_Fit_Para(hist, work_dir+histX_pkl, work_dir+histY_pkl)
    para = Fit.Fit_sgl_Gauss(hist, p[0], p[1], p[2], [min_c-50, max_c+50], work_dir+para_pkl)
    """
    X = pd.read_pickle(work_dir+histX_pkl)
    Y = pd.read_pickle(work_dir+histY_pkl)
    parameter = pd.read_pickle(work_dir+para_pkl)
    Fit.Fig_Plot_multi(X,Y, parameter, work_dir+chrg_fig)
    