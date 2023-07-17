import ROOT as RT
import numpy as np
import pickle
import sys
import pandas as pd

def Calc_AP_Charge(file, tree, av_wf_d, AP_df, name_np, branch_t = "time", branch_w = "wform", cell=1024):
    #AP_file avg_wf
    avg_wf = RT.std.vector(float)(pd.read_pickle(av_wf_d))
    #AP catarog
    AP_cat = pd.read_pickle(AP_df)
    event = RT.std.vector(int)(np.array(AP_cat["event"]))
    seg = RT.std.vector(int)(np.array(AP_cat["seg"]))
    chrg = RT.std.vector(float)()

    RT.CalcAPCharge2(file, tree, cell, avg_wf, event, seg, chrg, branch_t, branch_w)
    #AP_cat["charge"] = np.array(chrg)
    with open(name_np, "wb") as f:
        pickle.dump(np.array(chrg), f)
    print("##### Create chrg files #####")

    #return AP_cat
    
def Calc_AP_Charge2(file, tree, av_wf_d, AP_df, name_np, branch_t = "time", branch_w = "wform", cell=1024):
    #AP_file avg_wf
    avg_wf = RT.std.vector(float)(pd.read_pickle(av_wf_d))
    #AP catarog
    AP_cat = pd.read_pickle(AP_df)
    RS = AP_cat[AP_cat["Label"] != 0]
    event = RT.std.vector(int)(np.array(RS["event"]))
    seg = RT.std.vector(int)(np.array(RS["peak_cell"]))
    chrg = RT.std.vector(float)()

    RT.CalcAPCharge2(file, tree, cell, avg_wf, event, seg, chrg, branch_t, branch_w)
    #AP_cat["charge"] = np.array(chrg)
    with open(name_np, "wb") as f:
        pickle.dump(np.array(chrg), f)
    print("##### Create chrg files #####")

    #return AP_cat
    

def Calc_AP_Charge_K(file, tree, av_wf_d, AP_df, name_np, branch_t = "time", branch_w = "wform", cell = 1024):
    #AP_file avg_wf
    avg_wf = RT.std.vector(float)(pd.read_pickle(av_wf_d))
    #AP catarog
    AP_cat = pd.read_pickle(AP_df)
    RS = AP_cat[AP_cat["Label"] != 0]
    eve = RT.std.vector(int)(np.array(RS["event"]))
    seg_s = RT.std.vector(int)(np.array(RS["Start_cell"]))
    seg_e = RT.std.vector(int)(np.array(RS["end_cell"]))
    chrg = RT.std.vector(float)()
    RT.CalcAPCharge_K2(file, tree, cell, avg_wf, eve, seg_s, seg_e, chrg, branch_t, branch_w)
    #RS["charge"] = np.array(chrg)
    
    with open(name_np, "wb") as f:
        pickle.dump(np.array(chrg), f)
    print("##### Create chrg files #####")