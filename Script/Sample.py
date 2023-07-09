import pymodule.IO.Convert_DRS4 as DRS4
import pymodule.Avg_wf.Calc_Avg_Wf as AVG
import pymodule.Charge_analysis.Charge_Dist as Chrg
import pymodule.Fitting.Fitting as Fit
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
    file = args[1]
    dfile = file
    tr = "Treesource_0"
    tr_d = "Treedark_0"

    #working
    work_dir = args[2]

    #Output
    new_file = "new.root"
    avg_s_pkl = "avg_s.pkl"
    avg_d_pkl = "avg_d.pkl"
    avg_fig = "Avg.png"
    chrg_pkl = "chrg.pkl"
    chrg_fig = "Chrg.png"
    histX_pkl = "histX_pkl"
    histY_pkl = "histY_pkl"
    para_pkl = "para.pkl"
    
    #get the pass
    cwd = os.getcwd()


    create = DRS4.Create_ROOT_file_2386(file, file, work_dir, work_dir+new_file, 10, tr, tr_d)
    del create
    avg_wf = AVG.Calc_Avg_Wf_all(work_dir+new_file, work_dir+new_file, work_dir+avg_s_pkl, work_dir+avg_d_pkl)
    int_max  = AVG.Plot_Avg(avg_wf[1], avg_wf[2], work_dir+avg_fig)
    #print(str(int_max))
    #int_max = 370
    del avg_wf
    del file
    charge = Chrg.Calc_Charge(work_dir+new_file, work_dir+new_file, int_max - 25, int_max + 50, work_dir+chrg_pkl)
    hist = Chrg.Plot_Hist(charge, -20, 120)
    p = Fit.Find_Fit_Para(hist, work_dir+histX_pkl, work_dir+histY_pkl)
    para = Fit.Fit_trpl_Gauss(hist, p[0], p[1], p[2], [-20, 100], work_dir+para_pkl)
    del hist, charge, p, para
    X = pd.read_pickle(work_dir+histX_pkl)
    Y = pd.read_pickle(work_dir+histY_pkl)
    parameter = pd.read_pickle(work_dir+para_pkl)
    #Fit.Fig_Plot(p[3],p[4], para[0], work_dir+chrg_fig)
    Fit.Fig_Plot(X,Y, parameter, work_dir+chrg_fig)
    
    print("Done!!")